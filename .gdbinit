file bin/kernel.bin
target remote localhost:1234
#set disassembly-flavor intel
#layout split
define plist
  set var $n = $arg0
  while $n
    printf "Size: %d, Address: %p, Next: %p, Is next bigger than me? %d\n", $n->size, $n, $n->next, $n->next > $n
    set var $n = $n->next
  end
end
define pp 
  plist Heap::holes_list_head
end

gef config context.layout "-legend regs stack args memory -threads extra code trace source"
