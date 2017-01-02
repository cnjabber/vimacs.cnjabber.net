Reverse engineering findakey
============================

original url of findakey: http://www.reversing.be/easyfile/file.php?show=20051210213037366

`findakey <findakey.tar.bz2>`_ is a program which is written in C++, statically linked and stripped. So it's not so easy to reverse engineering it and find a key to it. I'll show how to analyze this program and write a keygen for it.

For security, I recommend using another user to run this program or run it in a virtual machine.

To analyze this program, I use `radare2 <https://radare.org>`_ to do the disassembly job, and `gdb <https://sourceware.org/gdb/>`_ to trace the program.

Load the program
----------------

First, I open the program with radare2::

  r2 findakey

Then radare2 will show a prompt ``[0x08048100]>``, which means the program has been successfully loaded, and the entry point is 0x08048100. Use ``pd`` command, we'll see the instructions the program executes. One thing we can find is::

  0x08048117      68f0810408     push 0x80481f0 ; main
  0x0804811c      e837320400     call 0x808b358

That means radare2 can analyze programs linked with C libraries such as glibc, it knows the structure of the C runtime library so it can point out which is the ``main()`` function. So now let's go to the main function and see what happens. Use command ``s 0x80481f0`` and we can go to this address. But before continuing, I'd like to run a command ``aaa`` to make radare2 to analyze more things about this program, then do more disassembly.

We can see a lot of calls. This program is written in C++, so it calls a lot of constructors and destructors. We need to guess what kind of class the local variables points to.

At first, the large amount of function calls confuses me. I can analyze some functions which actually do nothing. What I can find is:

* local_18h, local_28h, local_38h are constructed with fcn.080719c0, which increment a global counter and initialize them with an address.
* local_58h, local_78h are constructed with fcn.0806f1d0, which actually does nothing
* fcn.08071f30 constructs local_48h with an empty C string, and local_58h. This constructor calls fcn.030a7c60, which is actually strlen() after analyzing.
* fcn.08071f30 also constructs local_68h with C string "-".

After all these constructors, finally I can see the program outputs strings. It's easy to guess fcn.0806d000 does things like std::cout and fcn.0806aa20 does things like std::cin, and we can see the first name is stored in local_18h and the last name is stored in local_28h.

How strings are stored
----------------------

So I think it's time to use gdb to see how this program store the two strings I input. We set a breakpoint at 0x08048352 after which the program has read the input.::

  (gdb) set disassembly-flavor intel
  (gdb) set disassemble-next-line on
  (gdb) b *0x08048352
  Breakpoint 1 at 0x8048352
  (gdb) r
  Starting program: /home/evil/findakey
  Please enter your FirstName: Iru
  Please enter your LastName: Cai

  Breakpoint 1, 0x08048352 in ?? ()
  => 0x08048352:83 c4 10add    esp,0x10
  (gdb) x $ebp-0x18
  0xffffd4a0:0x08114314
  (gdb) x/16c 0x08114314
  0x8114314:73 'I'114 'r'117 'u'0 '\000'3 '\003'0 '\000'0 '\000'0 '\000'
  0x811431c:3 '\003'0 '\000'0 '\000'0 '\000'0 '\000'0 '\000'0 '\000'0 '\000'

Great. In this program, the each string object stores a pointer to a memory location which stores the C string of the string value.

Now let's go back to radare2. After reading two strings, the program does some testing::

  |     ||    0x08048358      8d45e8         lea eax, [ebp - local_18h]
  |     ||    0x0804835b      50             push eax
  |     ||    0x0804835c      e89fa20200     call fcn.08072600
  |     ||    0x08048361      83c410         add esp, 0x10
  |     ||    0x08048364      85c0           test eax, eax
  |     ||,=< 0x08048366      745f           je 0x80483c7

And at 0x80483c7::

  |  |``--`-> 0x080483c7      83ec08         sub esp, 8
  |  |  ||    0x080483ca      6810ce0608     push 0x806ce10
  |  |  ||    0x080483cf      83ec0c         sub esp, 0xc
  |  |  ||    0x080483d2      68485d0e08     push str.I_expect_a_capital_letter_in_the_first_letter__and_to_be_larger_than_1_char. ; str.I_expect_a_capital_letter_in_the_first_letter__and_to_be_larger_than_1_char. ; "I expect a capital letter in the first letter, and to be larger than 1 char." @ 0x80e5d48
  |  |  ||    0x080483d7      6880e81008     push 0x810e880
  |  |  ||    0x080483dc      e81f4c0200     call fcn.0806d000

So it seems that fcn.08072600 is something like ``string::size()``. We can analyze it::

  [0x080481f0]> pdf @ fcn.08072600
  / (fcn) fcn.08072600 13
  |   fcn.08072600 (int arg_8h);
  |           ; arg int arg_8h @ ebp+0x8
  |           ; XREFS: CALL 0x0804835c  CALL 0x0804836f  CALL 0x080484c5  CALL 0x0804857d  CALL 0x080485c5  CALL 0x080486a5
  |           ; XREFS: CALL 0x080486c9  CALL 0x080487db
  |           0x08072600      55             push ebp
  |           0x08072601      89e5           mov ebp, esp
  |           0x08072603      8b4508         mov eax, dword [ebp + arg_8h] ; [0x8:4]=0
  |           0x08072606      5d             pop ebp
  |           0x08072607      8b00           mov eax, dword [eax]
  |           0x08072609      8b40f4         mov eax, dword [eax - 0xc]
  \           0x0807260c      c3             ret

So, from fcn.08072600 we know that the length of the string is 12 bytes behind the C string. We can also confirm it with gdb::

  (gdb) x/w 0x08114314-0xc
  0x8114308:0x00000003

This program also check the first letter of the two input strings. It uses fcn.08072c30 to get the C string part (with offset). This function will also be used later.

The core part
-------------

Now we go to 0x08048456, the place where the program jumps to after finishing checking the first name and last name.::

  |  `------> 0x08048456      83ec08         sub esp, 8
  |     |||   0x08048459      8d4588         lea eax, [ebp - local_78h]
  |     |||   0x0804845c      83ec0c         sub esp, 0xc
  |     |||   0x0804845f      8d55d8         lea edx, [ebp - local_28h]
  |     |||   0x08048462      52             push edx
  |     |||   0x08048463      8d55e8         lea edx, [ebp - local_18h]
  |     |||   0x08048466      52             push edx
  |     |||   0x08048467      50             push eax
  |     |||   0x08048468      e889060000     call fcn.08048af6
  |     |||   0x0804846d      83c414         add esp, 0x14
  |     |||   0x08048470      8d4588         lea eax, [ebp - local_78h]
  |     |||   0x08048473      50             push eax
  |     |||   0x08048474      8d45c8         lea eax, [ebp - local_38h]
  |     |||   0x08048477      50             push eax
  |     |||   0x08048478      e8539c0200     call fcn.080720d0
  |     |||   0x0804847d      83c410         add esp, 0x10
  |    ,====< 0x08048480      eb26           jmp 0x80484a8

We can trace with gdb and guess fcn.08048af6 concatenates local_18h and local_28h to local_78h. Then fcn.080720d0 duplicates local_78h to local_38h, and I don't know why it does this. Then at 0x80484a8, the program calls fcn.08072070, which I guess is the destructor of the string class.::

  |   |`----> 0x080484a8      83ec0c         sub esp, 0xc
  |   | |||   0x080484ab      8d4588         lea eax, [ebp - local_78h]
  |   | |||   0x080484ae      50             push eax
  |   | |||   0x080484af      e8bc9b0200     call fcn.08072070
  |   | |||   0x080484b4      83c410         add esp, 0x10
  |   | |||   0x080484b7      c74580000000.  mov dword [ebp - local_80h], 0
  |   | |||   ; JMP XREF from 0x0804856a (main)
  |   |.----> 0x080484be      83ec0c         sub esp, 0xc
  |   |||||   0x080484c1      8d45c8         lea eax, [ebp - local_38h]
  |   |||||   0x080484c4      50             push eax
  |   |||||   0x080484c5      e836a10200     call fcn.08072600
  |   |||||   0x080484ca      83c410         add esp, 0x10
  |   |||||   0x080484cd      394580         cmp dword [ebp - local_80h], eax ; [0x13:4]=256
  |  ,======< 0x080484d0      7205           jb 0x80484d7
  | ,=======< 0x080484d2      e998000000     jmp 0x804856f
  | |||||||   ; JMP XREF from 0x080484d0 (main)
  | |`------> 0x080484d7      83ec0c         sub esp, 0xc

Then local_80h is set to 0, and then a ``cmp``, a ``jb`` and a ``jmp``, a loop is here.

Let's move forward to 0x804856f, which is the core part::

  | `-------> 0x0804856f      c74580000000.  mov dword [ebp - local_80h], 0
  |   | |||   ; JMP XREF from 0x0804864d (main)
  |   |.----> 0x08048576      83ec0c         sub esp, 0xc
  |   |||||   0x08048579      8d45c8         lea eax, [ebp - local_38h]
  |   |||||   0x0804857c      50             push eax
  |   |||||   0x0804857d      e87ea00200     call fcn.08072600
  |   |||||   0x08048582      83c410         add esp, 0x10
  |   |||||   0x08048585      394580         cmp dword [ebp - local_80h], eax ; [0x13:4]=256
  |  ,======< 0x08048588      7205           jb 0x804858f
  | ,=======< 0x0804858a      e9c3000000     jmp 0x8048652
  | |||||||   ; JMP XREF from 0x08048588 (main)
  | |`------> 0x0804858f      83ec08         sub esp, 8
  | | |||||   0x08048592      ff7580         push dword [ebp - local_80h]
  | | |||||   0x08048595      8d45c8         lea eax, [ebp - local_38h]
  | | |||||   0x08048598      50             push eax
  | | |||||   0x08048599      e892a60200     call fcn.08072c30
  | | |||||   0x0804859e      83c410         add esp, 0x10
  | | |||||   0x080485a1      0fbe10         movsx edx, byte [eax]
  | | |||||   0x080485a4      8d4584         lea eax, [ebp - local_7ch]
  | | |||||   0x080485a7      0110           add dword [eax], edx
  | | |||||   0x080485a9      8b5580         mov edx, dword [ebp - local_80h]
  | | |||||   0x080485ac      42             inc edx
  | | |||||   0x080485ad      8b4584         mov eax, dword [ebp - local_7ch]
  | | |||||   0x080485b0      89d1           mov ecx, edx
  | | |||||   0x080485b2      99             cdq
  | | |||||   0x080485b3      f7f9           idiv ecx
  | | |||||   0x080485b5      89857cffffff   mov dword [ebp - local_84h], eax
  | | |||||   0x080485bb      83ec08         sub esp, 8
  | | |||||   0x080485be      83ec04         sub esp, 4
  | | |||||   0x080485c1      8d45c8         lea eax, [ebp - local_38h]
  | | |||||   0x080485c4      50             push eax
  | | |||||   0x080485c5      e836a00200     call fcn.08072600
  | | |||||   0x080485ca      83c408         add esp, 8
  | | |||||   0x080485cd      2b4580         sub eax, dword [ebp - local_80h]
  | | |||||   0x080485d0      48             dec eax
  | | |||||   0x080485d1      50             push eax
  | | |||||   0x080485d2      8d45c8         lea eax, [ebp - local_38h]
  | | |||||   0x080485d5      50             push eax
  | | |||||   0x080485d6      e855a60200     call fcn.08072c30
  | | |||||   0x080485db      83c410         add esp, 0x10
  | | |||||   0x080485de      8a00           mov al, byte [eax]
  | | |||||   0x080485e0      88857bffffff   mov byte [ebp - local_85h], al
  | | |||||   0x080485e6      83ec08         sub esp, 8
  | | |||||   0x080485e9      8a857cffffff   mov al, byte [ebp - local_84h]
  | | |||||   0x080485ef      32857bffffff   xor al, byte [ebp - local_85h]
  | | |||||   0x080485f5      660fbed0       movsx dx, al
  | | |||||   0x080485f9      89d0           mov eax, edx
  | | |||||   0x080485fb      b95d000000     mov ecx, 0x5d               ; ']'
  | | |||||   0x08048600      6699           cwd
  | | |||||   0x08048602      66f7f9         idiv cx
  | | |||||   0x08048605      88d0           mov al, dl
  | | |||||   0x08048607      83c021         add eax, 0x21               ; '!'
  | | |||||   0x0804860a      0fbec0         movsx eax, al
  | | |||||   0x0804860d      50             push eax
  | | |||||   0x0804860e      8d45b8         lea eax, [ebp - local_48h]
  | | |||||   0x08048611      50             push eax
  | | |||||   0x08048612      e839a70200     call fcn.08072d50
  | | |||||   0x08048617      83c410         add esp, 0x10
  | | |||||   0x0804861a      83ec08         sub esp, 8
  | | |||||   0x0804861d      8a857cffffff   mov al, byte [ebp - local_84h]
  | | |||||   0x08048623      660fbed0       movsx dx, al
  | | |||||   0x08048627      89d0           mov eax, edx
  | | |||||   0x08048629      b91a000000     mov ecx, 0x1a
  | | |||||   0x0804862e      6699           cwd
  | | |||||   0x08048630      66f7f9         idiv cx
  | | |||||   0x08048633      88d0           mov al, dl
  | | |||||   0x08048635      83c061         add eax, 0x61               ; 'a'
  | | |||||   0x08048638      0fbec0         movsx eax, al
  | | |||||   0x0804863b      50             push eax
  | | |||||   0x0804863c      8d45b8         lea eax, [ebp - local_48h]
  | | |||||   0x0804863f      50             push eax
  | | |||||   0x08048640      e80ba70200     call fcn.08072d50
  | | |||||   0x08048645      83c410         add esp, 0x10
  | | |||||   0x08048648      8d4580         lea eax, [ebp - local_80h]
  | | |||||   0x0804864b      ff00           inc dword [eax]
  | | |`====< 0x0804864d      e924ffffff     jmp 0x8048576
  | | | |||   ; JMP XREF from 0x0804858a (main)
  | `-------> 0x08048652      83ec08         sub esp, 8
  |   | |||   0x08048655      8b5584         mov edx, dword [ebp - local_7ch]
  |   | |||   0x08048658      89d0           mov eax, edx
  |   | |||   0x0804865a      b95d000000     mov ecx, 0x5d               ; ']'
  |   | |||   0x0804865f      99             cdq
  |   | |||   0x08048660      f7f9           idiv ecx
  |   | |||   0x08048662      8d4221         lea eax, [edx + 0x21]       ; 0x21 ; '!'
  |   | |||   0x08048665      0fbec0         movsx eax, al
  |   | |||   0x08048668      50             push eax
  |   | |||   0x08048669      8d4598         lea eax, [ebp - local_68h]
  |   | |||   0x0804866c      50             push eax
  |   | |||   0x0804866d      e8dea60200     call fcn.08072d50

We can see some code is duplicate here, but don't worry, because we finally see the full loop here. We know local_80h is the loop counter. For the functions in the loop, we already know that fcn.08072600 is ``string::size()``, and fcn.08072c30 returns the pointer to the C string part at some offset. The remaining function we don't know is fcn.08072d50. However, after tracing the program to see the content in local_48h, we can guess fcn.08072d50 is to append a character to a string. After the loop, a character is also appended to the string pointed by local_68h, which is originally "-".

What's the serial?
------------------

After computing the string at local_48h and local_68h, the program asks for a serial, which stores at local_58h. Then checks whether the string is longer than 2 bytes. If not, the program fails to register. If the serial is longer than 2 bytes::

  |   |||||   0x080486b6      83ec08         sub esp, 8
  |   |||||   0x080486b9      8d4598         lea eax, [ebp - local_68h]
  |   |||||   0x080486bc      50             push eax
  |   |||||   0x080486bd      83ec04         sub esp, 4
  |   |||||   0x080486c0      6a02           push 2
  |   |||||   0x080486c2      83ec08         sub esp, 8
  |   |||||   0x080486c5      8d45a8         lea eax, [ebp - local_58h]
  |   |||||   0x080486c8      50             push eax
  |   |||||   0x080486c9      e8329f0200     call fcn.08072600
  |   |||||   0x080486ce      83c40c         add esp, 0xc
  |   |||||   0x080486d1      83e802         sub eax, 2
  |   |||||   0x080486d4      50             push eax
  |   |||||   0x080486d5      8d45a8         lea eax, [ebp - local_58h]
  |   |||||   0x080486d8      50             push eax
  |   |||||   0x080486d9      8d4588         lea eax, [ebp - local_78h]
  |   |||||   0x080486dc      50             push eax
  |   |||||   0x080486dd      e8debc0200     call fcn.080743c0
  |   |||||   0x080486e2      83c410         add esp, 0x10
  |   |||||   0x080486e5      8d4588         lea eax, [ebp - local_78h]
  |   |||||   0x080486e8      50             push eax
  |   |||||   0x080486e9      e860040000     call fcn.08048b4e
  |   |||||   0x080486ee      83c410         add esp, 0x10
  |   |||||   0x080486f1      88856fffffff   mov byte [ebp - local_91h], al
  |  ,======< 0x080486f7      eb26           jmp 0x804871f

We can see a fcn.080743c0 here. After a ``pdf @ fcn.080743c0``, I can see the string ``str.basic_string::substr`` in the disassembly output, so it must be the substring function. We can also use gdb to see the value of local_78h, and know it's really the last two bytes of local_58h, the serial I input.

There's also a fcn.08048b4e. We can easily disassemble it and find that it's comparing two strings.

Now comes the tricky part. We can see that fcn.08048b4e is to compare two strings. The first one is obvious, local_78h, but what about the second one. We have to emulate the stack operation. We must notice that fcn.080743c0, the substr function, returns with ``ret 4`` instruction, which means it'll add 4 to ``esp`` after returns. After all these, we know the second string to compare is local_68h, the two byte string starting with character '-'.

Now let's move to the second part::

  | |`------> 0x0804871f      83ec0c         sub esp, 0xc
  | | |||||   0x08048722      8d4588         lea eax, [ebp - local_78h]
  | | |||||   0x08048725      50             push eax
  | | |||||   0x08048726      e845990200     call fcn.08072070
  | | |||||   0x0804872b      83c410         add esp, 0x10
  | | |||||   0x0804872e      80bd6fffffff.  cmp byte [ebp - local_91h], 0
  | |,======< 0x08048735      0f848f000000   je 0x80487ca

It destructs local_78h, compares the two strings above, then go to 0x80487ca if the two strings are identical.::

  | |`------> 0x080487ca      83ec08         sub esp, 8
  | | |||||   0x080487cd      8d45b8         lea eax, [ebp - local_48h]
  | | |||||   0x080487d0      50             push eax
  | | |||||   0x080487d1      83ec04         sub esp, 4
  | | |||||   0x080487d4      83ec0c         sub esp, 0xc
  | | |||||   0x080487d7      8d45a8         lea eax, [ebp - local_58h]
  | | |||||   0x080487da      50             push eax
  | | |||||   0x080487db      e8209e0200     call fcn.08072600
  | | |||||   0x080487e0      83c410         add esp, 0x10
  | | |||||   0x080487e3      83e802         sub eax, 2
  | | |||||   0x080487e6      50             push eax
  | | |||||   0x080487e7      6a00           push 0
  | | |||||   0x080487e9      8d45a8         lea eax, [ebp - local_58h]
  | | |||||   0x080487ec      50             push eax
  | | |||||   0x080487ed      8d4d88         lea ecx, [ebp - local_78h]
  | | |||||   0x080487f0      51             push ecx
  | | |||||   0x080487f1      e8cabb0200     call fcn.080743c0
  | | |||||   0x080487f6      83c410         add esp, 0x10
  | | |||||   0x080487f9      8d4588         lea eax, [ebp - local_78h]
  | | |||||   0x080487fc      50             push eax
  | | |||||   0x080487fd      e84c030000     call fcn.08048b4e
  | | |||||   0x08048802      83c410         add esp, 0x10
  | | |||||   0x08048805      88856effffff   mov byte [ebp - local_92h], al
  | |,======< 0x0804880b      eb26           jmp 0x8048833

This is very similar. It compares local_48h with the serial we input, without the last two bytes.

Let's look forward to 0x8048833. We can see if these two strings are also the same, then the program is registered.

The keygen
----------

Now we can write a keygen according to the analysis. The big loop that calculates the serial may be a little complicated, but is not so hard if we know some assembly, and we can write this loop in less than 10 lines of C code.

This keygen can be seen at `keygen.c <keygen.c>`_.
