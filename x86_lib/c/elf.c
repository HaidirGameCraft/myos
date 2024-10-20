#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <elf.h>
#include <page.h>

uint8_t elf_check_file(elf32_header* header) {
    if( !header ) return 0;

    if( header->e_ident[0] != ELFMAGIC0) {
        println("Error> ELF Header ELF_MAGIC0 incorrent");
        return 0;
    } 
    if ( header->e_ident[1] != ELFMAGIC1) {
        println("Error> ELF Header ELF_MAGIC1 incorrent");
        return 0;
    } 
    if ( header->e_ident[2] != ELFMAGIC2) {
        println("Error> ELF Header ELF_MAGIC2 incorrent");
        return 0;
    }
     if ( header->e_ident[3] != ELFMAGIC3) {
        println("Error> ELF Header ELF_MAGIC3 incorrent");
        return 0;
    }

    return 1;
}

uint8_t elf_check_supported(elf32_header* header) {
    if( !elf_check_file( header ) ) {
        println("Invalid ELF file.");
        return 0;
    }

    if( header->e_ident[EI_CLASS] != ELFCLASS32 ) {
        println("Unsupported ELF Class");
        return 0;
    }
    if( header->e_ident[EI_DATA] != ELFDATA2LSB ) {
        println("Unsupported ELF byte order");
        return 0;
    }
    if( header->e_machine != EM_386 ) {
        println("Unsupported EM 386 ");
        return 0;
    }
    if( header->e_ident[EI_VERSION] != EV_CURRENT ) {
        println("Unsupported Ev Version");
        return 0;
    }
    if( header->e_type != ET_REL && header->e_type != ET_EXEC ) {
        println("Unsupported ELF file type");
        return 0;
    }

    return 1;
}

/*
void *elf_load_rel(elf32_header* hdr) {
    int r = 0;
    r = elf_load_stage1( hdr );
    if( r == ELF_RELOC_ERR ) {
        println("Unable to load ELF file.");
        return NULL;
    }

    r = elf_load_stage2( hdr );
    if( r == ELF_RELOC_ERR ) {
        println("Unable to load ELF file.");
        return NULL;
    }

    return ( void* )hdr->e_entry;
}
*/

void* elf_load_file(void* file) {
    elf32_header* hdr = (elf32_header*) file;

    if( !elf_check_supported( hdr ) ) {
        println("Error file cannot be loaded.");
        return NULL;
    }

    elf32_pheader *program = (elf32_pheader*)((uint8_t*) file + hdr->e_phoff);

    // println("\n");
    // printf("ELF Program: \n");
    // printf("Virtual Memory: 0x%x\n", program->p_vaddr);
    // printf("Physical Memory: 0x%x\n", program->p_paddr);
    // printf("Physical Header Length: 0x%x\n", hdr->e_phnum);
    // printf("Sizeof ELF32 Physical Header: %i\n", sizeof( elf32_pheader ));

    for(int i = 0; i < hdr->e_phnum; i++) {
        if( program[i].p_type == 1 ) {
            void *segment = (void*) program[i].p_vaddr;
            void *elf_segment = ((uint8_t*) file) + program[i].p_offset;

            // printf("ELF Program Header: %i \n", i);
            // printf("Virtual Memory: 0x%x\n", program[i].p_vaddr);
            // printf("Physical Memory: 0x%x\n", program[i].p_paddr);
            // printf("ELF Segment: 0x%x \n Size Memory: %i \n", program[i].p_offset, program[i].p_memsz);
            // printf("ELF File Size: %i\n", program[i].p_filesz);

            SetVirtualMapping((uint32_t) elf_segment, (uint32_t) segment, program[i].p_filesz);
            //memset(segment, 0, program[i].p_filesz);
            memcpy(segment, elf_segment, program[i].p_filesz);

            if( program[i].p_memsz > program[i].p_filesz ) {
                printf("p memsz is bigger than p filesize\n");
                memset((uint8_t*) segment + program[i].p_filesz, 0, program[i].p_memsz - program[i].p_filesz);
            }
        }
    }

    //switch( hdr->e_type ) {
    //    case ET_EXEC:
    //        return NULL;
    //    case ET_REL:
    //        return elf_load_rel( hdr );
    //}

    return (void*) hdr->e_entry;
}

void elf_clear(void* file) {
    elf32_header *hdr = (elf32_header*) file;
    elf32_pheader *program = (elf32_pheader*)((uint8_t*) file + hdr->e_phoff);


    for(int i = 0; i < hdr->e_phnum; i++) {
        if( program[i].p_type == 1 ) {
            void *segment = (void*) program[i].p_vaddr;
            void *elf_segment = ((uint8_t*) file) + program[i].p_offset;

            printf("Clearing virtual map\n");
            memset(segment, 0, program[i].p_filesz);
            ClearVirtualMapping( (uint32_t) segment, program[i].p_filesz);
            if( program[i].p_memsz > program[i].p_filesz ) {
                memset((uint8_t*) segment + program[i].p_filesz, 0, program[i].p_memsz - program[i].p_filesz);
            }
        }
    }
}

/*
ELF32_Shdr* elf_sheader(ELF32_Header* hdr) {
    return (ELF32_Shdr*)((int) hdr + hdr->e_shoff);
}
ELF32_Shdr* elf_section(ELF32_Header* hdr, int index) {
    return &elf_sheader(hdr)[index];
}

inline char *elf_str_table(ELF32_Header* hdr) {
    if( hdr->e_shstrndx == SHN_UNDEF )
        return NULL;

    return (char*) hdr + elf_section(hdr, hdr->e_shstrndx)->sh_offset;
}
inline char *elf_lookup_string(ELF32_Header* hdr, int offset) {
    char *str_tab = elf_str_table( hdr );

    if( str_tab == NULL )
        return NULL;

    return str_tab + offset;
}
int elf_get_symbolVal(ELF32_Header* hdr, int table, uint32_t index) {
    if( table == SHN_UNDEF || index == SHN_UNDEF) return NULL;

    ELF32_Shdr *sym_tab = elf_section( hdr, table );

    uint32_t sym_tab_entries = sym_tab->sh_size / sym_tab->sh_entsize;

    if( index >= sym_tab_entries ) {
        println("Error> Index Out Of Range");
        return ELF_RELOC_ERR;
    }

    int sym_addr = (int) hdr + sym_tab->sh_offset;
    ELF32_Symbol *symbol = &((ELF32_Symbol*) sym_addr)[ index ];

    if( symbol->st_shndx == SHN_UNDEF ) {
        ELF32_Shdr *strtab = elf_section( hdr, sym_tab->sh_link);

        const char *name = (const char*) hdr + strtab->sh_offset + symbol->st_name;
    }
    else if ( symbol->st_shndx == SHN_ABS ) {
        return symbol->st_value;
    } else {
        ELF32_Shdr *target = elf_section( hdr, symbol->st_shndx );
        return (int) hdr + symbol->st_value + target->sh_offset;
    }

    return ELF_RELOC_ERR;
}

int elf_load_stage1(ELF32_Header * hdr) {
    ELF32_Shdr *shdr = elf_sheader( hdr );

    for(uint32_t i = 0; i < hdr->e_shnum; i++) {
        ELF32_Shdr *section = &shdr[i];
        if(section->sh_type == SHT_NOBITS ) {
            if( !section->sh_size ) continue;

            if( section->sh_flags & SHF_ALLOC ) {
                void *mem = malloc(section->sh_size);
                memset(mem, 0, section->sh_size);

                section->sh_offset = (int) mem - (int) hdr;

                char *value = itos( section->sh_size );
                print("Allocate memory for a section ");
                println( value );
                free( value );
            }
        }
    }

    return 0;
}
int elf_load_stage2(ELF32_Header * hdr) {
    ELF32_Shdr *shdr = elf_sheader( hdr );

    uint32_t index;
    for(uint32_t i = 0; i < hdr->e_shnum; i++) {
        ELF32_Shdr *section = &shdr[i];

        if( section->sh_type == SHT_REL ) {
            for(index = 0; index < section->sh_size / section->sh_entsize; index++) {
                ELF32_Rel *reltab = &((ELF32_Rel*)((int) hdr + section->sh_offset))[ index ];
                int result = elf_do_reloc( hdr, reltab, section );

                if( result == ELF_RELOC_ERR ) {
                    println("Failed to relocat symbol");
                    return ELF_RELOC_ERR;
                }
            }
        }
    }

    return 0;
}

#define DO_386_32(S, A) ((S) + (A))
#define DO_386_PC32(S, A, P) ((S) + (A) - (P))

int elf_do_reloc(ELF32_Header* hdr, ELF32_Rel* rel, ELF32_Shdr* section) {
    ELF32_Shdr *target = elf_section(hdr, section->sh_info);

    int addr = (int) hdr + target->sh_offset;
    int *ref = (int *)(addr + rel->r_offset);

    uint8_t *c = malloc(20);

    int symval = 0;
    if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
        symval = elf_get_symbolVal( hdr, section->sh_link, ELF32_R_SYM(rel->r_info));

        if( symval == ELF_RELOC_ERR ) 
            return ELF_RELOC_ERR;

        switch (ELF32_R_TYPE(rel->r_info)) {
            case R_386_NONE:
                break;
            case R_386_32:
                *ref = DO_386_32(symval, *ref);
                break;
            case R_386_PC32:
                *ref = DO_386_PC32(symval, *ref, (int) ref);
                break;
            default:
                println("Unsupported Relocation Type");
                return ELF_RELOC_ERR;
        }

    }
    return symval;
}
*/
