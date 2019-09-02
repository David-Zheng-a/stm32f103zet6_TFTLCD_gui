#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED
#include "sys.h"



//////////////////////////////////////////////////////////////////////////////////	 
//仅支持240*320的触摸屏
//推荐ALIENTEK精英STM32F103开发板
//创建日期 2019/9/2
//版本 v1.0
//作者 ZYK
//********************************************************************************
//初级布局简易使用说明
//首先定义一个_gui_GridLayout* 变量，使用creatLayout()函数为其赋值 ：例如_gui_GridLayout* layout; layout = creatLayout(xxx, xxx, xxx, xxx, xxx, xxx);
//然后定义n个_button* 变量，使用creatButton()给其赋值，注意：id不能为0xFFFF, 0, 0xFFFE, 0xFFFD，此id为保留地址
//调用addView()将_button*添加至_gui_GridLayout* 中的可用位置
//调用showLayout()将配置好的_gui_GridLayout*显示到屏幕上
//在循环中调用GridLayout_scan()扫描某一个layout并返回按钮id
//之后可以根据id进行其他操作
//
//弹窗简易使用说明
//需要弹窗时定义一个_gui_GridLayout* 变量，使用showDia()函数为其赋值，将会在屏幕中间显示一个简易弹窗
//使用GridLayout_scan()扫描弹窗
//确认按键id将会返回0xFFFE，取消将会返回0xFFFD。
//根据id进行下一步操作
//注：弹窗不会自动消失，需要调用disLayout()，和desLayout()先销毁弹窗及其数据，之后再通过showLayout()将原布局显示
//********************************************************************************
//////////////////////////////////////////////////////////////////////////////////	 




//button结构体，存储按钮或者文本的基础数据
typedef struct
{
	u16 id;//id
	u8 *text;//要显示的文字
	u8 enTouch;//是否显示边框，并可以被按下
	u16 xraw;//x格
	u16 yraw;//y格
	u16 xmin;//x坐标最小值
	u16 ymin;//y坐标最小值
	u16 textColor;//文字颜色
	u16 backColor;//背景颜色
} _button;


//layout结构体，存储布局的基础信息
typedef struct
{
	_button* contains[100];
	u16 viewNumbers;//包含的view个数
	u16 xmin;//x最小值
	u16 ymin;
	u16 xmax;//x最大值
	u16 ymax;
	u16 xraw;//x格数
	u16 yraw;
	u8 showAble;//是否显示
} _gui_GridLayout;

extern u16 BACKCOLOR; //全局背景颜色
extern _button *pos, *neg; // dialog的两个按钮

void GUI_Init(u16 Color); //初始化GUI，参数为全局背景颜色

_gui_GridLayout* creatLayout(u16 xs,u16 ys,u16 xe,u16 ye,u16 xr,u16 yr);//参数：起始x，起始y，结束x，结束y，x轴分割格数，y轴分割格数

_button* creatButton(u16 id,u8* text,u8 ct, u16 textColor, u16 backColor);//创建新的view，参数：id，显示的内容，是否触摸

void addView(_button*,_gui_GridLayout*,u16 xr,u16 yr);//向布局中添加view,参数：view，layout，x位置，y位置

u8 showLayout(_gui_GridLayout*);//显示某一个layout

_gui_GridLayout* showDia(u8 *title, u8 *text);//创建并显示一个dialog

u16 GridLayout_scan(_gui_GridLayout* layout);//扫描某个layout是否被触摸

void disLayout(_gui_GridLayout*);//使某个layout消失

void desLayout(_gui_GridLayout* layout);//彻底消除某个layout及其内部按钮，并将所用内存释放。调用此函数之前请先调用disLayout。



#endif // GUI_H_INCLUDED
