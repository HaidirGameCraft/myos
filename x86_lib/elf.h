#ifndef __ELF_HEADER__
#define __ELF_HEADER__

#include <stdint.h>

#define ELFMAGIC0 0x7F
#define ELFMAGIC1 'E'
#define ELFMAGIC2 'L'
#define ELFMAGIC3 'F'

#define ELF_RELOC_ERR -1

#define ELFDATA2LSB (1)
#define ELFCLASS32 (1)

typedef struct {
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} elf32_header;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} elf32_sheader;

typedef struct {
    uint32_t st_name;
    uint32_t st_value;
    uint32_t st_size;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
} elf32_symbol;

typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
} elf32_rel;

typedef struct {
    uint32_t r_offset;
    uint32_t r_info;
    int32_t r_addend;    
} elf32_rela;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} elf32_pheader;

#define ELF32_R_SYM(INFO) ((INFO) >> 8)
#define ELF32_R_TYPE(INFO) ((uint8_t)(INFO))

enum RtT_Types {
    R_386_NONE,
    R_386_32,
    R_386_PC32
};



#define ELF32_ST_BIND(INFO)((INFO) >> 4)
#define ELF32_ST_TYPE(INFO)((INFO) & 0x0F)

enum StT_Bindings {
    STB_LOCAL,
    STB_GLOBAL,
    STB_WEAK
};

enum StT_types {
    STT_NOTYPE,
    STT_OBJECT,
    STT_FUNC
};

#define SHN_UNDEF (0x00)
#define SHN_ABS 0xFFF1

enum Sht_Types {
    SHT_NULL,
    SHT_PROGBITS,
    SHT_SYMTAB,
    SHT_STRTAB,
    SHT_RELA,
    SHT_NOBITS,
    SHT_REL
};

enum Sht_Attributes {
    SHF_WRITE = 0x01,
    SHF_ALLOC = 0x02
};

enum ELF_Ident {
    EI_MAGIC0 = 0,
    EI_MAGIC1,
    EI_MAGIC2,
    EI_MAGIC3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
    EI_OSABI,
    EI_ABIVERSION,
    EI_PAD,
};

enum ELF_Type {
    ET_NONE = 0,
    ET_REL,
    ET_EXEC
};

#define EM_386 (3)
#define EV_CURRENT (1)

uint8_t elf_check_file(elf32_header* header);
uint8_t elf_check_supported(elf32_header* header);
void *elf_load_rel(elf32_header* hdr);
void *elf_load_file(void* file);
void elf_clear(void* file);

/*
int elf_load_stage1(ELF32_Header * hdr);
int elf_load_stage2(ELF32_Header * hdr);
int elf_do_reloc(ELF32_Header* hdr, ELF32_Rel* rel, ELF32_Shdr* section);

ELF32_Shdr* elf_sheader(ELF32_Header* hdr);
ELF32_Shdr* elf_section(ELF32_Header* hdr, int index );

inline char *elf_str_table(ELF32_Header* hdr);
inline char *elf_lookup_string(ELF32_Header* hdr, int offset);
int elf_get_symbolVal(ELF32_Header* hdr, int table, uint32_t index);
*/

#endif
