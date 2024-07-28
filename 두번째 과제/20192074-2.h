#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <elf.h>

bool is_elf(Elf64_Ehdr eh);
void read_elf_header(int32_t fd, Elf64_Ehdr *elf_header);
void print_elf_header(Elf64_Ehdr elf_header);
char *read_section(int32_t fd, Elf64_Shdr sh);
void print_section_headers(int32_t fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[]);

// 단어 변경 함수 원형 추가
void changeWord(int32_t fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[]);

//Implement this function to check whether an input file is an elf file or not.
bool is_elf(Elf64_Ehdr elf_header)
{
	if(!strncmp((char*)elf_header.e_ident, "\177ELF", 4)){
		return true;
	}else{
		printf("It's not ELF file\n");
		return false;
	}
}

void read_elf_header(int32_t fd, Elf64_Ehdr *elf_header)
{
	assert(elf_header != NULL);
	assert(lseek(fd, (off_t)0, SEEK_SET) == (off_t)0);
	assert(read(fd, (void *)elf_header, sizeof(Elf64_Ehdr)) == sizeof(Elf64_Ehdr));
}

void print_elf_header(Elf64_Ehdr elf_header)
{
	printf("Storage class\t= ");
	switch(elf_header.e_ident[EI_CLASS])
	{
		case ELFCLASS32:
			printf("32-bit objects\n");
			break;

		case ELFCLASS64:
			printf("64-bit objects\n");
			break;

		default:
			printf("Unknwon CLASS\n");
			break;
	}

	printf("Data format\t= ");
	switch(elf_header.e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
			printf("2's complement, little endian\n");
			break;

		case ELFDATA2MSB:
			printf("2's complement, big endian\n");
			break;

		default:
			printf("Unknwon Format\n");
			break;
	}

	printf("OS ABI\t\t= ");
	switch(elf_header.e_ident[EI_OSABI])
	{
		case ELFOSABI_SYSV:
			printf("UNIX System V ABI\n");
			break;

		case ELFOSABI_HPUX:
			printf("HP-UX\n");
			break;

		case ELFOSABI_NETBSD:
			printf("NetBSD\n");
			break;

		case ELFOSABI_LINUX:
			printf("Linux\n");
			break;

		case ELFOSABI_SOLARIS:
			printf("Sun Solaris\n");
			break;

		case ELFOSABI_AIX:
			printf("IBM AIX\n");
			break;

		case ELFOSABI_IRIX:
			printf("SGI Irix\n");
			break;

		case ELFOSABI_FREEBSD:
			printf("FreeBSD\n");
			break;

		case ELFOSABI_TRU64:
			printf("Compaq TRU64 UNIX\n");
			break;

		case ELFOSABI_MODESTO:
			printf("Novell Modesto\n");
			break;

		case ELFOSABI_OPENBSD:
			printf("OpenBSD\n");
			break;

		case ELFOSABI_ARM_AEABI:
			printf("ARM EABI\n");
			break;

		case ELFOSABI_ARM:
			printf("ARM\n");
			break;

		case ELFOSABI_STANDALONE:
			printf("Standalone (embedded) app\n");
			break;

		default:
			printf("Unknown (0x%x)\n", elf_header.e_ident[EI_OSABI]);
			break;
	}

	printf("Filetype \t= ");
	switch(elf_header.e_type)
	{
		case ET_NONE:
			printf("N/A (0x0)\n");
			break;

		case ET_REL:
			printf("Relocatable\n");
			break;

		case ET_EXEC:
			printf("Executable\n");
			break;

		case ET_DYN:
			printf("Shared Object\n");
			break;
		default:
			printf("Unknown (0x%x)\n", elf_header.e_type);
			break;
	}

	printf("Machine\t\t= ");
	switch(elf_header.e_machine)
	{
		case EM_NONE:
			printf("None (0x0)\n");
			break;

		case EM_386:
			printf("INTEL x86 (0x%x)\n", EM_386);
			break;

		case EM_X86_64:
			printf("AMD x86_64 (0x%x)\n", EM_X86_64);
			break;

		case EM_AARCH64:
			printf("AARCH64 (0x%x)\n", EM_AARCH64);
			break;

		default:
			printf(" 0x%x\n", elf_header.e_machine);
			break;
	}
	printf("\n");
}

char *read_section(int32_t fd, Elf64_Shdr sh)
{
	char* buff = malloc(sh.sh_size);
	if(!buff) {
		printf("%s:Failed to allocate %ldbytes\n",
				__func__, sh.sh_size);
	}

	assert(buff != NULL);
	assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
	assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size);

	return buff;
}

void print_section_headers(int32_t fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[])
{
	uint32_t i;
	char* sh_str;

	assert(lseek(fd, (off_t)eh.e_shoff, SEEK_SET) == (off_t)eh.e_shoff);

	for(i=0; i<eh.e_shnum; i++) {
		assert(read(fd, (void *)&sh_table[i], eh.e_shentsize) == eh.e_shentsize);
	}

	/* section-header string-table */
	sh_str = read_section(fd, sh_table[eh.e_shstrndx]);
	for(i=0; i<eh.e_shnum; i++) {
		if(!strncmp((sh_str + sh_table[i].sh_name), ".rodata", 7))
		{
			printf("%s section info\n", (sh_str + sh_table[i].sh_name));
			printf("    file offset = 0x%08lx\n", sh_table[i].sh_offset);
			printf("           size = 0x%08lx\n", sh_table[i].sh_size);
		}
	}
}

void changeWord(int32_t fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[])
{
	uint32_t i;
	char* sh_str;

	// ElF 헤더의 e_shoff 필드 값 만큼 파일 포인터를 이동
	assert(lseek(fd, (off_t)eh.e_shoff, SEEK_SET) == (off_t)eh.e_shoff);

	// section header table을 읽어 sh_table에 저장
	for (i = 0; i < eh.e_shnum; i++)
	{
		assert(read(fd, (void *)&sh_table[i], eh.e_shentsize) == eh.e_shentsize);
	}

	// 섹션 헤더 문자열 테이블을 읽음.
	sh_str = read_section(fd, sh_table[eh.e_shstrndx]);

	// .rodata section을 찾음
	for (i = 0; i < eh.e_shnum; i++)
	{
		// rodata section을 찾으면
		if (!strncmp((sh_str + sh_table[i].sh_name), ".rodata", 7))
		{
			// i번째 인덱스의 offset 저장
			off_t section_offset = sh_table[i].sh_offset;
			// i번째 인덱스의 sh_size 저장 
			size_t section_size = sh_table[i].sh_size;
			// section size 만큼 메모리 할당
			char *section_content = malloc(section_size);
			// offset 만큼 파일 포인터 이동
			assert(lseek(fd, section_offset, SEEK_SET) == section_offset);
			// 메모리에 로드
			assert(read(fd, section_content, section_size) == section_size);

			// section_content를 가리키는 포인터 정의
			char *ptr = section_content;
			// section_size를 내에 반복문을 돌면서
			for (size_t i=0; i < section_size; i++) 
			{
				// software 문자열을 찾으면
				if (memcmp(ptr+i, "software", 8) == 0)
				{
					// ptr+i를 hacker!로 변경
					memcpy(ptr+i, "hackers!", 8);
					// offset 만큼 파일 포인터를 이동 시킴
					assert(lseek(fd, section_offset, SEEK_SET) == section_offset);
					// 파일을 다시 씀
					assert(write(fd, section_content, section_size) == section_size);
					break;
				}
			}
			free(section_content);
		}
	}
}
