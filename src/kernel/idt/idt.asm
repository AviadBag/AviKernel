idtr DW 0 ; For limit storage
     DD 0 ; For base storage

global install_idt
install_idt:
    MOV   EAX, [esp + 4]
    MOV   [idtr + 2], EAX
    MOV   AX, [ESP + 8]
    MOV   [idtr], AX
    LIDT  [idtr]

    ret