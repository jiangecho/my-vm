my-vm
=====

Many tiems, I have noting to do in the office, and we call this situation as 'DaJiangYou'. But I really do not like daing JiangYou, so I want to do something intersting. 
Maybe writing a 'virtual machine' is an intersting thing, so I try to write a stack-based virtual machine. But I think it too difficult for me to write a 
high language virtual machine at present, so I decided to define my own instruction set, write the related assember and a small virtual machine.

The instructions will have the following feature:
1. the instruction set is stack-based. Because it is easier to implement the assembler
2. the instruction set will include some strange instructions, such as THREAD 
which will create a new thread to excute the following method.

this is an assembler and a virtual machine.
