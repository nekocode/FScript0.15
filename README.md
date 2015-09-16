# README

### Preview
![输入图片说明](art/preview.gif "在这里输入图片标题")

### DEMO
- **[远去之日的花](http://pan.baidu.com/s/1qWvGMQg)**
- **[历史版本 Package](http://pan.baidu.com/s/1c0HdWXE)** （包含以下内容）
  - FScript 0.2
  - 缘之空同人 0.15
  - [动漫音乐包] 0.15beta
  - 0.09beta 示例
  - 翻版缘之空 0.053
  - fs 0.04beta


### 分支以及相关系列说明
- **[Fscript 0.1x 系列](https://github.com/nekocode/FScript0.15)**：
采用自定义 `fscript` 语法驱动引擎

```
*init_echo
{
//这一段用于初始化echo和rolename框
epic "pic\sys\echo.png",0,400
epos 28,498,0
efontsize 25
ewidth 720
ecolor 255,255,255

rpic "pic\sys\none.png",5,415
rpos 55,465,0
rfontsize 25
rwidth 200
rcolor 255,255,255
}

*show_logo
{
wait 500
bk "pic\sys\logo.jpg"
wait 3000
clbk
}


call init_echo
//call show_logo

playm "music\1.-遠い空へ- (ヨスガノソラ メインテーマ).ogg"
bk "pic\bk\b40c.jpg"
"为什么我必须去承担这种痛苦呢!"
"父母在一场车祸中遇难了,留下了我和穹,而之后的世界仿佛没有了我们的容身之所,变得那么的陌生...."
clecho
```

- **[Fscript 0.2x 系列](https://github.com/nekocode/FScript0.2)**：
采用自定义修改后的 `lua` 语法驱动引擎

```
@fscript
-----[读取函数]-----
dofile("func\\load.txt")
dofile("func\\echo.txt")
dofile("func\\background.txt")
dofile("func\\role.txt")

nextfile("1.txt");
```

```
@fscript
bk("b1")
rl("穹")
"这是一\n\s\"\\n个测试","外界宿"
bk("b2")
clecho()
clrl()
clbk()
stop()


"结尾到了"
:end
"0","1"

go(":end")

:end2
:end3
:end4
```

- **[Fscript IDE(for fs 0.1x)](https://github.com/nekocode/FScriptIDE)**：
通过拓展 `notepad++` 为 fs 引擎提供方便的 IDE
