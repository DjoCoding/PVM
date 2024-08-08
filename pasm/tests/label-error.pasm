; multiple label definition error

#main:
	jmp $label

#label:
	jmp $label

#label:
	halt
