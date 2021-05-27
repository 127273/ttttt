
cc = gcc 
ld = ld
as = as
nm = nm
objdump =objdump
objcopy = objcopy
nasm = nasm
qemu = qemu-system-x86_64
bochs = bochs

arch ?= x86_64
kernel := build/kernel-$(arch).bin
isoname := klika-os-$(arch).iso
iso := build/$(isoname)
linker_script := src/ldscripts/kernel.ld
grub_cfg := src/grub/grub.cfg

assembly_source_files := $(wildcard src/drivers/*.asm) $(wildcard src/kernel/*.asm) $(wildcard src/lib/*.asm) $(wildcard src/arch/x86_64/*.asm) \
$(wildcard src/hardware/*.asm)
assembly_object_files := $(patsubst src/%.asm, build/%.o, $(assembly_source_files))
nasm_flags := -w-number-overflow -felf64  -o0

SS_source_files := $(wildcard src/arch/x86_64/*.S)
SS_object_files := $(patsubst src/%.S, build/%.o, $(SS_source_files))
SS_flags := --64 

c_grub := 'src/grub'
c_hardware = 'src/hardware'
c_src := 'src'
c_include := 'src/include'
c_share := 'src/share'
cppflags := -fno-rtti -fno-use-cxa-atexit 
cflags := -fno-pic  -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		  -ffreestanding -o0 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
		  -nostdlib -fno-builtin  -fno-exceptions -fno-leading-underscore -Wno-write-strings \
	      -I $(c_include)  -I $(c_grub) -I $(c_share) -I $(c_hardware) -I $(c_src) -nostartfiles -nodefaultlibs -fno-exceptions \
	      -Wall -Wextra -c -mcmodel=large -Wno-implicit-fallthrough -O0 \
	      -Wno-parentheses -Wno-sign-compare -Wno-unused-parameter \
		  -Wno-address-of-packed-member  
		  #sign-compare 这个用户有符号和无符号数的比较警告
		  #address-of-packed-member   带有packed的结构体被类型强制转换的警告
#c_source_files := $(wildcard src/drivers/*.c) $(wildcard src/kernel/*.c) $(wildcard src/lib/*.c) $(wildcard src/arch/x86_64/*.c) $(wildcard src/syscalls/*.c)  
#c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))
cpp_source_files :=  $(wildcard src/drivers/*.cpp) $(wildcard src/kernel/*.cpp) $(wildcard src/lib/*.cpp) $(wildcard src/arch/x86_64/*.cpp) $(wildcard src/syscalls/*.cpp) $(wildcard src/grub/*.cpp) \
$(wildcard src/hardware/*.cpp) $(wildcard src/drivers/usb/*.cpp)  $(wildcard src/drivers/net/*.cpp)  $(wildcard src/drivers/net/i8254/*.cpp) 
cpp_object_files := $(patsubst src/%.cpp, \
	build/%.o, $(cpp_source_files))

#gcc -fno-pic  -fno-rtti -fno-use-cxa-atexit  -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2
quemu_mem := 256
hd_image = ../images/disk.img
usb_image = ../images/usb.img
.PHONY: all clean run iso install

all: $(kernel)

clean:
	@rm -rfv build

install:
	dd if=/dev/zero of=../images/disk.img count=81920 bs=512 #this is not small 
	mformat -F -v KLIKA-OS -i ../images/disk.img 
	mcopy -svn -i ../images/disk.img ../images/disk/* ::. 

run: $(iso)
	$(qemu) -cdrom $(iso) -m $(quemu_mem) -drive file=$(hd_image),format=raw,index=0,media=disk  -boot order=d \
	-device intel-hda -smp 2 -cpu core2duo  -name "Alice World Is Here" -usb \
	-drive if=none,format=raw,id=disk1,file=$(usb_image) \
	-device nec-usb-xhci,id=usb \
	-device usb-storage,bus=usb.0,drive=disk1  \
	-netdev user,id=net0,  -device e1000-82545em,netdev=net0,mac=DE:AD:69:BE:EF:42 -serial stdio #  -monitor stdio #  
#-enable-kvm  -cpu host
#qemu-system-x86_64 -net nic,model=?
run-bocsh: $(iso)
	$(objcopy) --only-keep-debug $(kernel) $(kernel).sym
	$(nm) $(kernel).sym | grep " T " | awk '{ print $$1" "$$3 }' > $(kernel).bochs.sym
	$(bochs) -rc ../bochs.rc -f ../bochs.cfg -q

run-qemu: $(iso)
	$(qemu) -cdrom $(iso) -m $(quemu_mem) -drive file=$(hd_image),format=raw,index=0,media=disk  -boot order=d -s -S \
	

iso: $(iso)

debug: nasm_flags += -g -F dwarf
debug: cflags += -g 
debug: all

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub     
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@cp $(iso) ../images/$(isoname)

$(kernel): $(assembly_object_files) $(cpp_object_files) $(SS_object_files)  $(linker_script)   #$(cpp_object_files)
	$(ld) -nostdlib -n -T $(linker_script) -o $(kernel) $(SS_object_files) $(assembly_object_files) $(cpp_object_files) # $(cpp_object_files)
	$(objdump) -D $(kernel) > build/kernel.dump.asm
	#$(objdump) -D build/arch/x86_64/APU_boot.o APU_boot.o
	#$(objdump) -x $(kernel) >> build/kernel.headers.txt

# compile assembly files
build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	$(nasm) $(nasm_flags) $< -o $@

# build/%.o: src/%.S
# 	@mkdir -p $(shell dirname $@)
# 	as --64  $< -o $@

# compile c files
#build/%.o: src/%.c
#	@mkdir -p $(shell dirname $@)
#	$(cc) $(cflags) $< -o $@

build/%.o: src/%.cpp
	@mkdir -p $(shell dirname $@)
	$(cc) $(cflags) $(cppflags) $< -o $@

