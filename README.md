# Sper

![](https://vhs.charm.sh/vhs-5POUPwNthd4Mh8ErxlYy5p.gif)

A simple C program that can generate and save license files from templates.

# Usage
> [!IMPORTANT]
> You have to set `SPER_LICENSE_DIR` environment variable to let Sper
> know where to find templates.
```
sper -h
```
Show help.
```
sper MIT
```
Load `MIT` template from `$SPER_LICENSE_DIR`.

# Parsing
In templates `<@key>` substrings are considered variables where `key` is the name of the
variable. At runtime Sper will ask you to fill your variables.

## Special Variables
`year` variable will default to the current year as its value.

# Install
> [!CAUTION]
> Only works on x86_64 Linux. It may work on 32 bit systems, but I haven't tested.
> Windows users can run Sper through WSL or Cygwin.

> [!NOTE]
> I cannot promise to maintain this project. I am very new to C so there most likely are 
bugs and security vulnerabilities in the program.

```
git clone --depth <REPO_URL>@main sper
cd sper
make RELEASE=1
./bin/sper
```
> [!IMPORTANT]
> You need a C compiler. I used GCC (`Ubuntu 11.4.0-1ubuntu1~22.04 11.4.0`) to build it.

> [!TIP]
> You can just do `make` to make debug builds.

> [!TIP]
> You can specify `INSTALL_DIR` variable then do `make install` to install Repa in that
directory. By default it is set to `$HOME/.local/bin`. Example: `sudo make install
INSTALL_DIR=/usr/local/bin RELEASE=1`

# Command-Line Arguments
```
Usage: sper [file]
note : [file] will be searched in '/home/myxi/Documents/licenses'
```
> [!NOTE]
> The path in the "note" is fetched from `$SPER_LICENSE_DIR`.
