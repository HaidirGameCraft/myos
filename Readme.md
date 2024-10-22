# MYOS
Hello, World From MYOS

This is my own project to making own OS ( Operating System ) from scratch. This project is inspire by ["NanoByte OS"](https://github.com/nanobyte-dev/nanobyte_os), [OSDev]( https://wiki.osdev.org/Expanded_Main_Page ) and [The Little OS Book](https://littleosbook.github.io/)

## Warning ( Read First )
<span style="color:red">"Dont Running this project from any Real Hardware or Real Machine!" It might be destroy your machine/PC/Laptop if you run this project in Real Machine. SO DONT RUN IT FROM REAL MACHINE"</span>.
<span style="color:yellow">"RUN THIS AT VIRTUAL MACHINE LIKE QEMU"</span>



## Requirement
### Linux :
```bash
apt-get install qemu
apt-get install make
```
### Linux (Fedora) :
```bash
dnf install qemu
dnf install make
```

## Installation & Compile Project
```bash
git clone -b myos-main https://github.com/HaidirGameCraft/myos.git
cd myos
make
```
```text
filename: MYOS.IMG
type: binary/raw binary
```

## Run Emulator
To run emulator, you should need Qemu, VMWare or any Virtual Machine (NOT Real Machine)
```bash
make run
```

## Conclusion
I hope this can improve my programming skills, knowledge and explore new things about OS.