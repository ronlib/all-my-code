.386                                    ; create 32 bit code
.model flat, stdcall                    ; 32 bit memory model
option casemap :none                    ; case sensitive


include c:\masm32\include\windows.inc
include c:\masm32\include\kernel32.inc


includelib c:\masm32\lib\kernel32.lib


; Proto's:

MemCopy 					proto Source:PTR BYTE,Dest:PTR BYTE,ln:DWORD
findImageMappedSize	proto unmappedImageBase : dword
copyHeaders					proto	unmappedImageBase : dword, allocatedSpace : dword
copySections				proto	unmappedImageBase : dword, allocatedSpace : dword
getHeadersSize				proto	unmappedImageBase : dword
fillImports						proto allocatedSpace : dword
fixRelocations				proto	allocatedSpace : dword
getDllMainAddress			proto allocatedSpace : dword



; Code

	.code



loadFromBuffer proc bufAddr:dword

	LOCAL allocatedMemorySize : dword
	LOCAL allocatedMemoryPtr : dword

	
	; Lets find out the size of the image, and copy the headers to it
	
	invoke findImageMappedSize, [bufAddr]
	
	; eax now holds the size of the image. Let's save it as a local, and allocate memory
	
	; Checking first that the PE header's value is okay
	cmp eax, 0
	je failed
	
	
	; Saving the needed memory size aside
	mov [allocatedMemorySize], eax	
	
	; Image size, as equired from the PE headers is greater than 0.
	
	invoke VirtualAlloc, NULL, eax, MEM_COMMIT, PAGE_EXECUTE_READWRITE
	cmp eax, NULL
	je failed
	
	; Memory allocation was successful. Copying it to a local variable
	mov [allocatedMemoryPtr], eax
	
	
	; Copying the headers
	invoke copyHeaders, [bufAddr], eax
	
	
	; Copying the sections
	invoke copySections, [bufAddr], [allocatedMemoryPtr]
	
	; Fill imports
	invoke fillImports, [allocatedMemoryPtr]
	
	; Handling relocations
	invoke fixRelocations, [allocatedMemoryPtr]
	
	jmp success
	
	free_memory:
	
	; Nothing to do if this function fails
	invoke VirtualFree, [allocatedMemoryPtr], [allocatedMemorySize], MEM_RELEASE
	mov eax, 0
	jmp failed
	
	success:
	
	mov eax, [allocatedMemoryPtr]
	
	failed:
			
	ret

loadFromBuffer endp




findImageMappedSize proc unmappedImageBase : dword

	LOCAL ntHeader : DWORD
	
	mov eax, [unmappedImageBase]
	assume eax: ptr IMAGE_DOS_HEADER	
	add eax, [eax].e_lfanew	
	assume eax: ptr IMAGE_NT_HEADERS	
	mov eax, [eax].OptionalHeader.SizeOfImage
	
	ret

findImageMappedSize endp




; ------------------------ getHeadersSize function ------------------------

getHeadersSize proc	unmappedImageBase : dword

	mov eax, [unmappedImageBase]
	assume eax: ptr IMAGE_DOS_HEADER	
	add eax, [eax].e_lfanew	
	assume eax: ptr IMAGE_NT_HEADERS	
	mov eax, [eax].OptionalHeader.SizeOfHeaders
	
	; eax now holds the total size of the headers
	
	ret
	
getHeadersSize endp

; ------------------------ /getHeadersSize function ------------------------



; ------------------------ copyHeaders function ------------------------

copyHeaders proc	unmappedImageBase : dword, 
							allocatedSpace : dword

	invoke getHeadersSize, [unmappedImageBase]
	
	; eax now holds the total size of the headers
	
	invoke MemCopy, [unmappedImageBase], [allocatedSpace], eax

	ret
	
copyHeaders endp

; ------------------------ /copyHeaders function ------------------------



; ------------------------ copySections function ------------------------
copySections proc unmappedImageBase : dword, 
							allocatedSpace : dword
	
	local numberOfSections : dword
	local unmappedImageSectionAddress : dword
	local mappedImageSectionAddress : dword
	local sectionSize : dword
	local sectionIndex : dword
	
	mov eax, [unmappedImageBase]
	assume eax: ptr IMAGE_DOS_HEADER	
	add eax, [eax].e_lfanew	
	assume eax: ptr IMAGE_NT_HEADERS
	xor ebx, ebx
	mov bx, [eax].FileHeader.NumberOfSections
	
	mov [numberOfSections], ebx
	
	
	; Moving eax by the size of IMAGE_NT_HEADERS in order to get to the IMAGE_SECTION_HEADERs table
	add eax, sizeof(IMAGE_NT_HEADERS)
	
	; Finding out where the section table begins
		
	assume eax : ptr IMAGE_SECTION_HEADER
	

	xor ecx, ecx
		
	; Looping through the sections, copying each one of them.
	
	
	mov [sectionIndex], 0
	
	copy_section_label:
	mov ecx, [sectionIndex]
	cmp ecx, [numberOfSections]
	jae finished_looping_label
	
	
	mov ebx, [unmappedImageBase]
	add ebx, [eax].PointerToRawData
	mov [unmappedImageSectionAddress], ebx
	mov ebx, [allocatedSpace]
	add ebx, [eax].VirtualAddress
	mov [mappedImageSectionAddress], ebx
	mov ebx, [eax].Misc.VirtualSize
	mov [sectionSize], ebx
	
	invoke MemCopy, [unmappedImageSectionAddress], [mappedImageSectionAddress], [sectionSize]
	
	add eax, sizeof(IMAGE_SECTION_HEADER)
	inc [sectionIndex]
	
	jmp copy_section_label
	
	
	finished_looping_label:
	
	ret
copySections endp
; ------------------------ /copySections function ------------------------





; ------------------------ fillImports function ------------------------

fillImports proc allocatedSpace : dword


	local importedFunctionIndex : dword
	local importedFunctionAddress : dword
	local loadedModuleAddress : dword
	local currentImageDescriptor : ptr IMAGE_IMPORT_DESCRIPTOR

	mov eax, [allocatedSpace]
	assume eax: ptr IMAGE_DOS_HEADER	
	add eax, [eax].e_lfanew
	assume eax: ptr IMAGE_NT_HEADERS
	
	lea eax, [eax].OptionalHeader.DataDirectory ; we aquire the address of the array
	assume eax: ptr IMAGE_DATA_DIRECTORY
	
	add eax, sizeof(IMAGE_DATA_DIRECTORY ) * IMAGE_DIRECTORY_ENTRY_IMPORT	
	mov ebx, [eax].VirtualAddress
	
	add ebx, [allocatedSpace]
	
	; ebx now holds the address of the import table
	mov [currentImageDescriptor], ebx
	
	; First we load the imported dll, and then we now go over the list of the imported functions,
	; finding them in the loaded dll, and assigning their addresses in the correct place.
	
	; Looping until [eax].Characteristics is NULL
	modules_loop:
		
		mov eax, [currentImageDescriptor]
		assume eax: ptr IMAGE_IMPORT_DESCRIPTOR 
		
		cmp [eax].Characteristics, NULL
		je modules_loop_end
		
		; Loading the module
		mov ebx, [allocatedSpace]
		add ebx, [eax].Name1
		
		invoke LoadLibraryA, ebx
		cmp eax, NULL
		; In case an error occured
		je modules_loop_end
		mov [loadedModuleAddress], eax
		
		
		; Let's go through the imported functions
		
		mov [importedFunctionIndex], 0		
		
		functions_loop:

			; Finding the imported function name
			mov eax, [currentImageDescriptor]
			mov ebx, [allocatedSpace]
			add ebx, [eax].Characteristics			
			; ebx now points to an array of IMAGE_IMPORT_BY_NAME pointers
			mov eax, [importedFunctionIndex]
			mov edx, 4
			mul edx
			add ebx, eax
			mov eax, [ebx]
			cmp eax, 0
			je functions_loop_end
			add eax, [allocatedSpace]

			assume eax: ptr IMAGE_IMPORT_BY_NAME
			lea eax, [eax].Name1

			; Found the imported function name. Now getting its address
					
			invoke GetProcAddress, [loadedModuleAddress], eax
			cmp eax, NULL
			; If the function could not be found, let's move on to the next module
			je functions_loop_end
			mov [importedFunctionAddress], eax
		

			; eax and importedFunctionAddress now holds the address of the imported function. We 
			; now assign that address to the correct index in 
			; IMAGE_IMPORT_DESCRIPTOR.FirstThunk (importedFunctionIndex)

			
			mov eax, [currentImageDescriptor]
			
			assume eax : ptr IMAGE_IMPORT_DESCRIPTOR
			
			mov ebx, [eax].FirstThunk
			add ebx, [allocatedSpace]
			
			mov eax, [importedFunctionIndex]
			xor ecx, ecx
			add ecx, 4
			mul ecx
			add ebx, eax
			
			; ebx now points to the right function placeholder
			
			mov eax, [importedFunctionAddress]
			mov [ebx], eax
			
			inc [importedFunctionIndex]
		
		jmp functions_loop
		
		functions_loop_end:
	
	add [currentImageDescriptor], sizeof (IMAGE_IMPORT_DESCRIPTOR)
	
	jmp modules_loop
	
	modules_loop_end:
	
	
	ret
	
fillImports endp


; ------------------------ /fillImports function ------------------------


; ------------------------ fixRelocations function ------------------------
fixRelocations 	proc	allocatedSpace : dword

	
	local relocSectionSize : dword
	local currentBlockRemainingRelocs : dword ; Current IMAGE_BASE_RELOCATION remaining relocations
	local delta : dword ; The difference between the address to which the image is mapped to, and the ImageBase field in the OptionalHeader struct
	local currentRelocationInformationBlockAddress : dword ; Current IMAGE_BASE_RELOCATION address we work with
	local firstRelocationInformationBlockAddress : dword ; First IMAGE_BASE_RELOCATION address we work with
	local relocationBlockAddress : dword ; The address of the block (4k in size) which we now relocate
	local currentSingleRelocationInformationAddress : dword ; Pointing at a WORD variable containing a single DWORD in memory that needs relocation
	
	
	; Aquiring .reloc section address and its size
	mov eax, [allocatedSpace]
	assume eax: ptr IMAGE_DOS_HEADER	
	add eax, [eax].e_lfanew
	assume eax: ptr IMAGE_NT_HEADERS
	
	mov ebx, [allocatedSpace]
	mov [delta], ebx
	mov ebx, [eax].OptionalHeader.ImageBase
	sub [delta], ebx
	
	lea eax, [eax].OptionalHeader.DataDirectory ; we aquire the address of the array
	assume eax: ptr IMAGE_DATA_DIRECTORY
	
	add eax, sizeof(IMAGE_DATA_DIRECTORY ) * IMAGE_DIRECTORY_ENTRY_BASERELOC		
	mov ebx, [eax].isize
	
	mov [relocSectionSize], ebx
	mov ebx, [eax].VirtualAddress
	add ebx, [allocatedSpace]
	

	mov [currentRelocationInformationBlockAddress], ebx
	mov [firstRelocationInformationBlockAddress], ebx	
	mov [currentSingleRelocationInformationAddress], 0
	
	; Starting iterating the relocation blocks
	iterate_relocation_blocks:
	
		; Checking if we went over all of the relocation data
		
		mov eax, [currentSingleRelocationInformationAddress]
		cmp eax, 0
		je single_block_relocation
		
		sub eax, [firstRelocationInformationBlockAddress]
		cmp eax, [relocSectionSize]		
		jae relocation_block_end_loop
		
		single_block_relocation:
		
			; Calculating the number of relocations, and pointing 
			; currentSingleRelocationInformationAddress at the first relocation info
			
			assume ebx : ptr IMAGE_BASE_RELOCATION
			mov ebx, [currentRelocationInformationBlockAddress]
			mov eax, [ebx].VirtualAddress
			add eax, [allocatedSpace]
			mov [relocationBlockAddress], eax
			
			; Calculating the number of relocations in this block by the formula:
			; (<size of relocation block information> - <size of IMAGE_BASE_RELOCATION) / <size of WORD>
			
			mov ebx, [ebx].SizeOfBlock
			sub ebx, sizeof(IMAGE_BASE_RELOCATION)
			shr ebx, 1 ; Dividing by 2 (sizeof word)
			mov [currentBlockRemainingRelocs], ebx
			
			mov ebx, [currentRelocationInformationBlockAddress]
			add ebx, sizeof(IMAGE_BASE_RELOCATION)
			mov [currentSingleRelocationInformationAddress], ebx
			
			
			inner_block_relocation: ; Relocations made according to IMAGE_BASE_RELOCATION
			
				cmp [currentBlockRemainingRelocs], 0
				je inner_block_relocation_end

				assume eax : ptr dword
				mov eax, [currentSingleRelocationInformationAddress]
				mov eax, [eax]
				and eax, 0000FFFFh
				shr ax, 12
				
				cmp ax, 3
				jne inner_block_relocation_next
				
				mov eax, [currentSingleRelocationInformationAddress]
				mov eax, [eax]
				and eax, 00000FFFh
				
				; ax now contains the RVA from the address pointed by the IMAGE_BASE_RELOCATION block
				
				add eax, [relocationBlockAddress]
				
				; eax now contains the address to be added the delta
				
				; Adding the delta
								
				mov ecx, [eax]
				add ecx, [delta]
				mov [eax], ecx
				
				inner_block_relocation_next:
				
				add [currentSingleRelocationInformationAddress], sizeof(word)
				dec [currentBlockRemainingRelocs]
				
				jmp inner_block_relocation
				
			inner_block_relocation_end:
		
		mov ebx, [currentRelocationInformationBlockAddress]
		add ebx, [ebx].SizeOfBlock
		mov [currentRelocationInformationBlockAddress], ebx
		
		jmp iterate_relocation_blocks
			
		
	relocation_block_end_loop:
	
	ret

fixRelocations endp

; ------------------------ /fixRelocations function ------------------------



; ------------------------ getDllMainAddress function ------------------------

getDllMainAddress proc allocatedSpace : dword

	mov eax, [allocatedSpace]
	assume eax: ptr IMAGE_DOS_HEADER	
	add eax, [eax].e_lfanew
	assume eax: ptr IMAGE_NT_HEADERS

	mov eax, [eax].OptionalHeader.AddressOfEntryPoint
	add eax, [allocatedSpace]
	
	
	ret

getDllMainAddress endp


; ------------------------ /getDllMainAddress function ------------------------

; End of code

end