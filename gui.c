#include "gui.h"
#include "malloc.h"	    
#include "lcd.h"
#include "text.h"
#include "touch.h"


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



u16 BACKCOLOR;



void GUI_Init(u16 Color){
	BACKCOLOR = Color;
	LCD_Clear(Color);
}



//创建一个新的layout
//参数：起始x，起始y，结束x，结束y，x轴格数，y轴格数
_gui_GridLayout* creatLayout(u16 xmi, u16 ymi, u16 xma, u16 yma, u16 xr, u16 yr){
	_gui_GridLayout* point;
	
	point = (_gui_GridLayout*) mymalloc(SRAMIN,sizeof(_gui_GridLayout));
	
	point->viewNumbers = 0;
	point->xmax = xma;
	point->xmin = xmi;
	point->ymax = yma;
	point->ymin = ymi;
	point->xraw = xr;
	point->yraw = yr;
	point->showAble = 1;
	
	return point;
}


//创建一个新的view
//参数：id，显示的内容，是否触摸
//注：id不能为0xFFFF, 0, 0xFFFE, 0xFFFD，此id为扫描按键保留地址
_button* creatButton(u16 idd, u8* textt, u8 ct, u16 tc, u16 bc){
	
	_button* point;	
	
	point = (_button*) mymalloc(SRAMIN,sizeof(_button));
	
	point->enTouch = ct;
	point->id = idd;
	point->text = textt;
	point->backColor = bc;
	point->textColor = tc;
	
	return point;
}


//向某一个layout添加view
//参数：view，layout，x位置，y位置
void addView(_button* view, _gui_GridLayout* layout, u16 xr, u16 yr){
	
	view->xraw = xr;
	view->yraw = yr;
	layout->contains[(layout->viewNumbers)] = view;
	(layout->viewNumbers) = (layout->viewNumbers) + 1;
	view->xmin = layout->xmin + xr * (layout->xmax - layout->xmin)/layout->xraw;
	view->ymin = layout->ymin + yr * (layout->ymax - layout->ymin)/layout->yraw;

	return;
}



//显示某个layout
//参数：layout指针
u8 showLayout(_gui_GridLayout* layout){
	
	u16 xas, yas, xsize, ysize, num, xtagert, ytagert;
	
	layout->showAble = 1;
	xas = layout->xmin;
	yas = layout->ymin;
	xsize = (layout->xmax - layout->xmin)/layout->xraw;
	ysize = (layout->ymax - layout->ymin)/layout->yraw;
	
	for (num=0; num < layout->viewNumbers; num++){
		xtagert = layout->contains[num]->xraw;
		ytagert = layout->contains[num]->yraw;
		LCD_Fill(xas + xtagert * xsize, yas + ytagert * ysize,xas + (xtagert + 1) * xsize, yas + (ytagert + 1) * ysize, layout->contains[num]->backColor);
		if(layout->contains[num]->enTouch){
			POINT_COLOR = BLACK;
			LCD_DrawRectangle(xas + xtagert * xsize, yas + ytagert * ysize,xas + (xtagert + 1) * xsize, yas + (ytagert + 1) * ysize);
		}
		POINT_COLOR = layout->contains[num]->textColor;
		Show_Str_Mid(xas + xtagert * xsize, yas + ytagert * ysize + ysize/2 - 8, layout->contains[num]->text, 16, xsize);	    			    	 
	}
	
	return 0;
}

//弹出一个提示窗，弹出后请手动恢复所有layout
_gui_GridLayout* showDia(u8 *title, u8 *text){
	
	_gui_GridLayout *layout;
	_button *blu1, *tib, *blu2, *texb;
	_button *pos, *neg;

	layout = creatLayout(10, 120, 230, 240, 3, 3);
	blu1 = creatButton(0xFFFF, "", 0, BLUE, BLUE);
	blu2 = creatButton(0xFFFF, "", 0, BLUE, BLUE);
	tib = creatButton(0xFFFF, title, 0, WHITE, BLUE);
	texb = creatButton(0xFFFF, text, 0, BLACK, WHITE);
	pos = creatButton(0xFFFE,"确定", 1, BLUE, WHITE);
	neg = creatButton(0xFFFD,"取消", 1, RED, WHITE);
	
	addView(blu1, layout, 0, 0);
	addView(tib, layout, 1, 0);
	addView(blu2, layout, 2, 0);
	addView(texb, layout, 1, 1);
	addView(pos, layout, 0, 2);
	addView(neg, layout, 2, 2);
	
	LCD_Clear(BACKCOLOR);	
	
	showLayout(layout);
	
	POINT_COLOR = RED;
	LCD_DrawRectangle(10, 120, 230, 240);
	
	return layout;
}



//扫描某个layout
u16 GridLayout_scan(	_gui_GridLayout* layout){
	
	u16 i;
	static u16 k = 0;
	static u16 key_pre = 0;
	static u16 last = 0XFFFF;
	u16 id = 0xFFFF;
	u16 xsize, ysize, xas, yas;
	
	xsize = (layout->xmax - layout->xmin)/layout->xraw;
	ysize = (layout->ymax - layout->ymin)/layout->yraw;
	xas = layout->xmin;
	yas = layout->ymin;
	
	if (layout->showAble){
		tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN){
				for(i=0; i<(layout->viewNumbers); i++){
					if((tp_dev.x[0]<(xas + ((layout->contains[i]->xraw)+1) * xsize))&&(tp_dev.x[0]>(xas + ((layout->contains[i]->xraw)) * xsize))&&(tp_dev.y[0]<(yas + ((layout->contains[i]->yraw)+1) * ysize))&&(tp_dev.y[0]>(yas + ((layout->contains[i]->yraw)) * ysize)) ){
						id = layout->contains[i]->id;
						k=i;
						break;
					}
				}
				if (id != 0xFFFF){
					if (id == key_pre){
							id=0XFFFF;
					}
					else{
						if (layout->contains[k]->enTouch){
							LCD_Fill(layout->contains[k]->xmin + 2, layout->contains[k]->ymin + 2, layout->contains[k]->xmin + xsize - 2, layout->contains[k]->ymin + ysize - 2, ~(layout->contains[k]->backColor));
							POINT_COLOR = layout->contains[k]->textColor;
							Show_Str_Mid(xas + (layout->contains[k]->xraw) * xsize, yas + (layout->contains[k]->yraw) * ysize + ysize/2 - 8, layout->contains[k]->text, 16, xsize);
						}
						key_pre = id;
					}
					if ((last != 0xFFFF)&&(last != k)){
						LCD_Fill(layout->contains[last]->xmin + 2, layout->contains[last]->ymin + 2, layout->contains[last]->xmin + xsize - 2, layout->contains[last]->ymin + ysize - 2, layout->contains[last]->backColor);
						POINT_COLOR = layout->contains[last]->textColor;
						Show_Str_Mid(xas + (layout->contains[last]->xraw) * xsize, yas + (layout->contains[last]->yraw) * ysize + ysize/2 - 8, layout->contains[last]->text, 16, xsize);
						last = k;
					}
					else{
						last = k;
					}
				}
			}
			else if(key_pre){
				LCD_Fill(layout->contains[k]->xmin + 2, layout->contains[k]->ymin + 2, layout->contains[k]->xmin + xsize - 2, layout->contains[k]->ymin + ysize - 2, layout->contains[k]->backColor);
				POINT_COLOR = layout->contains[k]->textColor;
				Show_Str_Mid(xas + (layout->contains[k]->xraw) * xsize, yas + (layout->contains[k]->yraw) * ysize + ysize/2 - 8, layout->contains[k]->text, 16, xsize);	    			    	 
				key_pre = 0;
				k = 0;
			}
		}
	
		return id;
}


void disLayout(_gui_GridLayout* layout){
	
	layout->showAble = 0;
	LCD_Fill(layout->xmin, layout->ymin, layout->xmax, layout->ymax, BACKCOLOR);
	
}

void desLayout(_gui_GridLayout* layout){
	
	u16 i = 0;
	
	for (i=0; i<(layout->viewNumbers); i++){
		myfree(SRAMIN, layout->contains[i]);
	}
	myfree(SRAMIN, layout);
}


