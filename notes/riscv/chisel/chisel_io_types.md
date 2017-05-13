Chisel的IO类型
==============

DecoupledIO
-----------

src/main/scala/chisel3/util/Decoupled.scala

```
/** A concrete subclass of ReadyValidIO signaling that the user expects a
  * "decoupled" interface: 'valid' indicates that the producer has
  * put valid data in 'bits', and 'ready' indicates that the consumer is ready
  * to accept the data this cycle. No requirements are placed on the signaling
  * of ready or valid.
  */
class DecoupledIO[+T <: Data](gen: T) extends ReadyValidIO[T](gen)
{
  override def cloneType: this.type = new DecoupledIO(gen).asInstanceOf[this.type]
}
```

而ReadyValidIO在同一个文件中定义

```
/** An I/O Bundle containing 'valid' and 'ready' signals that handshake
  * the transfer of data stored in the 'bits' subfield.
  * The base protocol implied by the directionality is that the consumer
  * uses the flipped interface. Actual semantics of ready/valid are
  * enforced via use of concrete subclasses.
  */
abstract class ReadyValidIO[+T <: Data](gen: T) extends Bundle
{
  val ready = Input(Bool())
  val valid = Output(Bool())
  val bits  = Output(gen.chiselCloneType)
}
```

于是可以看出，DecoupledIO中，有一个ready输入信号和一个valid输出信号。


input, output, flip
-------------------

chiselFrontend/src/main/scala/chisel3/core/Data.scala

```
sealed abstract class Direction(name: String) {
  override def toString: String = name
  def flip: Direction
}
object Direction {
  object Input  extends Direction("input") { override def flip: Direction = Output }
  object Output extends Direction("output") { override def flip: Direction = Input }
  object Unspecified extends Direction("unspecified") { override def flip: Direction = Input }
}
```

因此Chisel中有2种方向对象：Input和Output,并且flip是一个函数，返回相反方向的对象。

同时可以用Input和Output来定义信号的输入输出类型，这个可以在rocket-chip里面看到很多。

```
/**
* Input, Output, and Flipped are used to define the directions of Module IOs.
*
* Note that they currently clone their source argument, including its bindings.
*
* Thus, an error will be thrown if these are used on bound Data
*/
object Input {
  def apply[T<:Data](source: T)(implicit compileOptions: CompileOptions): T = {
    val target = source.chiselCloneType
    Data.setFirrtlDirection(target, Direction.Input)
    Binding.bind(target, InputBinder, "Error: Cannot set as input ")
  }
}
object Output {
  def apply[T<:Data](source: T)(implicit compileOptions: CompileOptions): T = {
    val target = source.chiselCloneType
    Data.setFirrtlDirection(target, Direction.Output)
    Binding.bind(target, OutputBinder, "Error: Cannot set as output ")
  }
}
object Flipped {
  def apply[T<:Data](source: T)(implicit compileOptions: CompileOptions): T = {
    val target = source.chiselCloneType
    Data.setFirrtlDirection(target, Data.getFirrtlDirection(source).flip)
    Binding.bind(target, FlippedBinder, "Error: Cannot flip ")
  }
}
```

asInput,asOutput的定义：

```
object Data {
  /**
  * This function returns true if the FIRRTL type of this Data should be flipped
  * relative to other nodes.
  *
  * Note that the current scheme only applies Flip to Elements or Vec chains of
  * Elements.
  *
  * A Bundle is never marked flip, instead preferring its root fields to be marked
  *
  * The Vec check is due to the fact that flip must be factored out of the vec, ie:
  * must have flip field: Vec(UInt) instead of field: Vec(flip UInt)
  */
  private[chisel3] def isFlipped(target: Data): Boolean = target match {
    case (element: Element) => element.binding.direction == Some(Direction.Input)
    case (vec: Vec[Data @unchecked]) => isFlipped(vec.sample_element)
    case (bundle: Bundle) => false
  }

  /** This function returns the "firrtl" flipped-ness for the specified object.
    *
    * @param target the object for which we want the "firrtl" flipped-ness.
    */
  private[chisel3] def isFirrtlFlipped(target: Data): Boolean = {
    Data.getFirrtlDirection(target) == Direction.Input
  }

  /** This function gets the "firrtl" direction for the specified object.
    *
    * @param target the object for which we want to get the "firrtl" direction.
    */
  private[chisel3] def getFirrtlDirection(target: Data): Direction = target match {
    case (vec: Vec[Data @unchecked]) => vec.sample_element.firrtlDirection
    case _ => target.firrtlDirection
  }

  /** This function sets the "firrtl" direction for the specified object.
    *
    * @param target the object for which we want to set the "firrtl" direction.
    */
  private[chisel3] def setFirrtlDirection(target: Data, direction: Direction): Unit = target match {
    case (vec: Vec[Data @unchecked]) => vec.sample_element.firrtlDirection = direction
    case _ => target.firrtlDirection = direction
  }

  implicit class AddDirectionToData[T<:Data](val target: T) extends AnyVal {
    def asInput(implicit opts: CompileOptions): T = {
      if (opts.deprecateOldDirectionMethods)
        Builder.deprecated("Input(Data) should be used over Data.asInput")
      Input(target)
    }
    def asOutput(implicit opts: CompileOptions): T = {
      if (opts.deprecateOldDirectionMethods)
        Builder.deprecated("Output(Data) should be used over Data.asOutput")
      Output(target)
    }
    def flip()(implicit opts: CompileOptions): T = {
      if (opts.deprecateOldDirectionMethods)
        Builder.deprecated("Flipped(Data) should be used over Data.flip")
      Flipped(target)
    }
  }
}
```
