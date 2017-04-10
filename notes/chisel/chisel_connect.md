Chisel中的连接
==============

我们在很多Chisel代码中看到``:=``和``<>``符号，它们是在 chiselFrontend/src/main/scala/chisel3/core/Data.scala 定义的。

注意scala里面一个接受一个参数的成员函数可以作为一个二元运算符使用。

```
/** This forms the root of the type system for wire data types. The data value
  * must be representable as some number (need not be known at Chisel compile
  * time) of bits, and must have methods to pack / unpack structured data to /
  * from bits.
  */
abstract class Data extends HasId {
  // Return ALL elements at root of this type.
  // Contasts with flatten, which returns just Bits
  private[chisel3] def allElements: Seq[Element]

  private[core] def badConnect(that: Data)(implicit sourceInfo: SourceInfo): Unit =
    throwException(s"cannot connect ${this} and ${that}")
  private[chisel3] def connect(that: Data)(implicit sourceInfo: SourceInfo, connectCompileOptions: CompileOptions): Unit = {
    if (connectCompileOptions.checkSynthesizable) {
      Binding.checkSynthesizable(this, s"'this' ($this)")
      Binding.checkSynthesizable(that, s"'that' ($that)")
      try {
        MonoConnect.connect(sourceInfo, connectCompileOptions, this, that, Builder.forcedModule)
      } catch {
        case MonoConnect.MonoConnectException(message) =>
          throwException(
            s"Connection between sink ($this) and source ($that) failed @$message"
          )
      }
    } else {
      this legacyConnect that
    }
  }
  private[chisel3] def bulkConnect(that: Data)(implicit sourceInfo: SourceInfo, connectCompileOptions: CompileOptions): Unit = {
    if (connectCompileOptions.checkSynthesizable) {
      Binding.checkSynthesizable(this, s"'this' ($this)")
      Binding.checkSynthesizable(that, s"'that' ($that)")
      try {
        BiConnect.connect(sourceInfo, connectCompileOptions, this, that, Builder.forcedModule)
      } catch {
        case BiConnect.BiConnectException(message) =>
          throwException(
            s"Connection between left ($this) and source ($that) failed @$message"
          )
      }
    } else {
      this legacyConnect that
    }
  }
  private[chisel3] def lref: Node = Node(this)
  private[chisel3] def ref: Arg = if (isLit) litArg.get else lref
  private[core] def cloneTypeWidth(width: Width): this.type
  private[chisel3] def toType: String
  private[core] def width: Width
  private[core] def legacyConnect(that: Data)(implicit sourceInfo: SourceInfo): Unit

  /** cloneType must be defined for any Chisel object extending Data.
    * It is responsible for constructing a basic copy of the object being cloned.
    * If cloneType needs to recursively clone elements of an object, it should call
    * the cloneType methods on those elements.
    * @return a copy of the object.
    */
  def cloneType: this.type

  /** chiselCloneType is called at the top-level of a clone chain.
    * It calls the client's cloneType() method to construct a basic copy of the object being cloned,
    * then performs any fixups required to reconstruct the appropriate core state of the cloned object.
    * @return a copy of the object with appropriate core state.
    */
  def chiselCloneType(implicit compileOptions: CompileOptions): this.type = {
    // Call the user-supplied cloneType method
    val clone = this.cloneType
    // In compatibility mode, simply return cloneType; otherwise, propagate
    // direction and flippedness.
    if (compileOptions.checkSynthesizable) {
      Data.setFirrtlDirection(clone, Data.getFirrtlDirection(this))
      //TODO(twigg): Do recursively for better error messages
      for((clone_elem, source_elem) <- clone.allElements zip this.allElements) {
        clone_elem.binding = UnboundBinding(source_elem.binding.direction)
        Data.setFirrtlDirection(clone_elem, Data.getFirrtlDirection(source_elem))
      }
    }
    clone
  }
  final def := (that: Data)(implicit sourceInfo: SourceInfo, connectionCompileOptions: CompileOptions): Unit = this.connect(that)(sourceInfo, connectionCompileOptions)
  final def <> (that: Data)(implicit sourceInfo: SourceInfo, connectionCompileOptions: CompileOptions): Unit = this.bulkConnect(that)(sourceInfo, connectionCompileOptions)
  def litArg(): Option[LitArg] = None
  def litValue(): BigInt = litArg.get.num
  def isLit(): Boolean = litArg.isDefined

  /** Returns the width, in bits, if currently known.
    * @throws java.util.NoSuchElementException if the width is not known. */
  final def getWidth: Int = width.get
  /** Returns whether the width is currently known. */
  final def isWidthKnown: Boolean = width.known
  /** Returns Some(width) if the width is known, else None. */
  final def widthOption: Option[Int] = if (isWidthKnown) Some(getWidth) else None

  // While this being in the Data API doesn't really make sense (should be in
  // Aggregate, right?) this is because of an implementation limitation:
  // cloneWithDirection, which is private and defined here, needs flatten to
  // set element directionality.
  // Related: directionality is mutable state. A possible solution for both is
  // to define directionality relative to the container, but these parent links
  // currently don't exist (while this information may be available during
  // FIRRTL emission, it would break directionality querying from Chisel, which
  // does get used).
  private[chisel3] def flatten: IndexedSeq[Bits]

  /** Creates an new instance of this type, unpacking the input Bits into
    * structured data.
    *
    * This performs the inverse operation of toBits.
    *
    * @note does NOT assign to the object this is called on, instead creates
    * and returns a NEW object (useful in a clone-and-assign scenario)
    * @note does NOT check bit widths, may drop bits during assignment
    * @note what fromBits assigs to must have known widths
    */
  def fromBits(that: Bits): this.type = macro CompileOptionsTransform.thatArg

  def do_fromBits(that: Bits)(implicit sourceInfo: SourceInfo, compileOptions: CompileOptions): this.type = {
    var i = 0
    val wire = Wire(this.chiselCloneType)
    val bits =
      if (that.width.known && that.width.get >= wire.width.get) {
        that
      } else {
        Wire(that.cloneTypeWidth(wire.width), init = that)
      }
    for (x <- wire.flatten) {
      x := bits(i + x.getWidth-1, i)
      i += x.getWidth
    }
    wire.asInstanceOf[this.type]
  }

  /** Packs the value of this object as plain Bits.
    *
    * This performs the inverse operation of fromBits(Bits).
    */
  @deprecated("Best alternative, .toUInt() or if Bits really needed, .toUInt().toBits()", "chisel3")
  def toBits(): UInt = SeqUtils.do_asUInt(this.flatten)(DeprecatedSourceInfo)

  /** Reinterpret cast to UInt.
    *
    * @note value not guaranteed to be preserved: for example, a SInt of width
    * 3 and value -1 (0b111) would become an UInt with value 7
    * @note Aggregates are recursively packed with the first element appearing
    * in the least-significant bits of the result.
    */
  final def asUInt(): UInt = macro SourceInfoTransform.noArg

  def do_asUInt(implicit sourceInfo: SourceInfo): UInt =
    SeqUtils.do_asUInt(this.flatten)(sourceInfo)

  // firrtlDirection is the direction we report to firrtl.
  // It maintains the user-specified value (as opposed to the "actual" or applied/propagated value).
  // NOTE: This should only be used for emitting acceptable firrtl.
  // The Element.dir should be used for any tests involving direction.
  private var firrtlDirection: Direction = Direction.Unspecified
  /** Default pretty printing */
  def toPrintable: Printable
}
```
