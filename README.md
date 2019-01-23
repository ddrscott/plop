# Plop Site

Plop a site on your desktop, best for videos, social media feeds, stock tickers, etc.

## Features
- Always on top (or not)
- Adjust opacity
- Scale view or font size
- Drag anywhere to move

## Build wxWidgets

```sh
# Download source from `https://www.wxwidgets.org/downloads/`
curl https://codeload.github.com/wxWidgets/wxWidgets/tar.gz/v3.0.4 -o wxWidgets-3.0.4.tar.gz

# Deflate
tar zxf wxWidgets-3.0.4.tar.gz

mkdir build
cd build

# Specific for OSX for now
../wxWidgets-3.0.4/configure --prefix `pwd` --disable-shared --disable-mediactrl --enable-universal_binary=i386,x86_64
```


## Build Plop

```sh
cd path/to/project

make
```
