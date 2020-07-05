---
title: Manjaro-System-configuran
date: 2020-04-03 12:43:11
index_img: /Picture/Manjaro.png
categories:
- 操作系统
tags:
- 操作系统
---
# Manjaro System finish picture

![Manjaro](manjaro_System_finish.png)

一下配置文件都在 https://github.com/LEXSSAMA/Configure-file-of-Manjaro-System


# 步骤
## 1.首先下载镜像安装

## ２．换源
```
sudo pacman-mirrors -i -c China -m rank
```
选择一个好用的就可以，我选择的是
https://mirrors.sjtug.sjtu.edu.cn/manjaro/

## ３．更新系统
```
sudo pacman -Syy 	#更新缓存
sudo Pacman -Syyu	#更新系统
```
## ４．配置VIM
```
sudo pacman -S vim
```
然后编辑$HOME下的.vim/vimrc文件即可<br>
我的也是刚刚开始使用vim所以配置的内容并不丰富
```
syntax on
set number
set relativenumber
set hlsearch
set smartcase
set cursorline
set cursorcolumn 
hi CursorColumn term=reverse ctermbg=white guibg=grey40
hi CursorColumn ctermbg=238 guibg=grey40
hi CursorLine term=underline cterm=underline guibg=grey40
set ignorecase
set cursorline
set wrap
set incsearch
set showcmd
set wildmenu
map s <nop>
map S :w<CR>
map Q :q<CR>
map R :source $MYVIMRC<CR>
noremap U 5k
noremap D 5j
call plug#begin('~/.vim/plugged')
Plug 'vim-airline/vim-airline'
Plug 'rakr/vim-one'
Plug 'preservim/nerdtree'
Plug 'ybian/smartim'
Plug 'arcticicestudio/nord-vim' Plug 'connorholyday/vim-snazzy'
Plug 'Valloric/YouCompleteMe'
Plug 'iamcco/markdown-preview.nvim', { 'do': 'cd app & yarn install'  }
Plug 'dhruvasagar/vim-table-mode', { 'on': 'TableModeToggle' }
Plug 'vimwiki/vimwiki'

call plug#end()	

let g:lightline = {
\ 'colorscheme': 'snazzy',
\ }

```
## 5.安装i3wm

```
sudo pacman -S i3
```
配置i3只需要配置~/.config/i3/config文件即可<br>
安装完i3可能会有分辨率的问题可以修改$HOME下的.Xresources文件
添加一下语句：<br>
```
Xft.dpi = 200 	#数字可以随便改
```

## 6.安装fcitx中文输入法
```
$ sudo pacman -S fcitx-sogoupinyin
$ sudo pacman -S fcitx-im     # 全部安装
$ sudo pacman -S fcitx-configtool     # 图形化配置工具
```
将fcitx添加到环境变量中去
```
sudo vim ~/.xprofile 
```
然后在里面添加：
```
export GTK_IM_MODULE=fcitx
export QT_IM_MODULE=fcitx
export XMODIFIERS=”@im=fcitx”
```
最后在~/.config/i3/config文件中添加
```
exec_always fcitx
```
这样开机就会直接启动fcitx了
## 7.安装alacritty
```
sudo pacman -S alacritty
```
在i3中修改alacritty的快捷键为mod+回车键
```
bindsym $mod+return exec alacritty
```
配置alacritty只需要修改~/.config/alacritty/alacritty.yml文件即可<br>

**如果~/.config/文件中没有alacritty/alacritty.yml文件就自己创建一个，或者上github上面复制一个就可以了**<br>
如果要alacritty的透明的话要安装picom，这个软件就是以前的compton
```
sudo pacman -S picom
```
执行picom,然后修改~/.config/alacritty/alacritty.yml<br>
```
background_opacity: 0.75 
```
也可以修改成其他的值，我比较喜欢0.75的透明度<br>
最后把picom加入i3的开机启动中



## 8.安装SSR
**无图像界面的SSR client**
```
wget http://www.texfox.com/ssr
sudo mv ssr /usr/local/bin
sudo chmod +x /usr/local/bin/ssr
ssr install
```
安装完成后配置SSR
```
ssr config
```
把机场的SSR信息填入
```
ssr start 
```
就连接上了，有不明白的命令可以执行
```
ssr help
```
**有图像界面的SSR client可以用electron-ssr**<br>
链接：
https://github.com/qingshuisiyuan/electron-ssr-backup
## 9.安装google-chrome_(谷歌浏览器)
```
sudo pacman -S google-chrome
```
因为很经常用到google所以我把它设置成i3的快捷键，即在~/.config/i3/config中添加下面语句
```
bindsym $mod+c exec google-chrome-stable 
```
## 10.安装Albert
因为i3默认的$mod+d的菜单快捷键太丑了，所以安装Albert 
![albert](albert.png)<br>
```
sudo pacman -S albert
```
在i3中设置albert开机启动<br>
```
exec_always albert
```
然后修改albert的热键在终端第一次执行albert时会提醒修改热键，下图是我的配置．<br>

![albert1](albert1.png)<br>
![albert2](albert2.png)<br>
因为我设置的热键ALT+d与i3的热键冲突（我的$mod键是ALT键）,所以我干脆取消了i3中的热键

## 11. 安装ranger
```
sudo pacman -S ranger
```
配置ranger依旧是修改~/.config/ranger/rc.conf文件<br>
**如果没有这些文件可以自己创建，详细看官方**
## 12. 安装网易云音乐
```
sudo pacman -S netease-cloud-music
```

## 13.安装QQ/TIM
```
sudo pacman -S deepin.qq.com.office
sudo pacman -S deepin.qq.com.im
```
由于在i3的环境下，一打开QQ就会闪退，解决方法是<br>
```
yaourt -S gnome-setting-daemon
```
然后运行/usr/lib/gsd-xsettings(最好在i3配置中设置开机启动)，然后就可以运行ＱＱ了


## 14. 安装light 
因为i3装上之后亮度调节是个问题所以装一个light
```
sudo pacman -S light
```
要设置亮度的时候只要在命令行输入
```
sudo light -S 20 	#数值可改，我一般用２０
```

