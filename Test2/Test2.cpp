
#include "stdafx.h"
#include<stdlib.h>
#include<stdio.h>
#include<GL\glut.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<iostream>
#include<Windows.h>
using namespace std;
#define ERASER 299
#define TOOLBAR 200

/*This values are _int8(which occupies 1 byte of memory)
 because they require only 0 or 1 value*/
__int8 countt=0,first=1,brush_type=1;

//func=0 do nothing
//func=1 Lines
//func=2 Lines Strip
//func=3 Pencil
//func=4 Unfilled Circle
//func=5 Trans Brush(Transperent Brush)
//func=6 Rectangle
//func=7 Text
//func=8 Eraser
//func=9 Spray Brush
//func=10 Opaque Brush
//func=11 Filled Circle
//func=12 Filled Rectangle
//func=13
//Specifies the current OPERATION.
__int8 func=0;

//store the current window size as width and height
int ww=600,hh=600;
float wx,hx;

/*drawing characters one after another,also to provide the space between each other.This variable increments the position
to draw the next character.if the user clicks on other place in TEXT OPERATION Mode this variable is set to zero for the 
first character*/
long typing_check_flag=20;

//Initial Values for COLOR PALETE position
int w[3]={510,540},h=400;

//store the values of x and y co-ordinates from mouse for temporarily for drawing LINES,LINES STRIP,CIRCLE,SQUARE
//previously selected vertex
int x2,y2;

//Rectangle co-ordinates
int rx[4]={-1},ry[4]={-1};

//set the brush size for BRUSH,LINE,POINT,ERASER
int brush_size=20,line_width=1,point_size=1;
float eraser_size=1;

//used to store the random values for BRUSH drawing
long a,b,c;

//Current Color for drawing
GLfloat cr=1,cg,cb;

//Radius while drawing a CIRCLE
GLfloat r;

//these vareiables used for Bitmap image processing
int depth,palete,compression,hsize,hoffset,hres,vres,planes,fsize,ds;

void init()
{
	glClearColor(1,1,1,1);
}


void save() 
{
	FILE *ip;
	#pragma warning(disable:4996)//to disable the fopen warning
	ip=fopen("C:\\users\\arun\\desktop\\itybnh.bmp","wb");
	//Identifier 2bytes
	fputc('B',ip);
	fputc('M',ip);
  	DWORD t=ww*hh*3+54;
	fwrite(&t,4,1,ip);//file size 6bytes
	t=0;
	fwrite(&t,4,1,ip);//reserved for later use(may be represent the Orientation) 10bytes
	t=54;
	fwrite(&t,4,1,ip);//data offset 14bytes
	t=40;
	fwrite(&t,4,1,ip);//header size 18bytes
	t=ww;
    fwrite(&t,4,1,ip);//width 22bytes
	t=hh;
	fwrite(&t,4,1,ip);//height 26bytes
	t=planes=1;
    fwrite(&t,2,1,ip);//number of planes 28bytes
    t=24;
    fwrite(&t,2,1,ip);//bits per pixel=24 30bytes
    t=0;
	fwrite(&t,4,1,ip);//compression=0 34bytes
	t=ww*hh*3;
	fwrite(&t,4,1,ip);//data size 38bytes
	t=0;
	fwrite(&t,4,1,ip);//Hresolution 42bytes
	t=0;
	fwrite(&t,4,1,ip);//Vresolution 46bytes
	t=0;
	fwrite(&t,4,1,ip);//Colors 50bytes
	t=0;
	fwrite(&t,4,1,ip);//Important Colors 54bytes

//since No color palete occurs for 24 bit depth image,proceeding to the bitmap data

float RGB[3];//array to hold the returned colors from the glReadPixels() function
unsigned __int8 BGR[3];//This type is integer with 1 byte memory
int i,j;
int f=(wx*500);
for(j=0;j<hh;j++)
{
	for(i=0;i<ww;i++)
	{
		glReadPixels(i,j,1,1,GL_RGB,GL_FLOAT,RGB);
		/*Bitmap requires BGR format and the color values should be integer with range 0-255.
		so,The Colors are Rearanged in BGR format to the array */
		BGR[0]=RGB[2]*255;//Blue
		BGR[1]=RGB[1]*255;//Green
		BGR[2]=RGB[0]*255;//Red
		fwrite(&BGR,3,1,ip);
	}
}
fclose(ip);	
} 
//function to draw a LINE
void draw_line(int x,int y)
{   
	
	glColor3f(cr,cg,cb);//draw with currently selected color
	glLineWidth(line_width);
	glBegin(GL_LINES);
	glVertex2f(x2,hh-y2);//previously selected vertex
	glVertex2f(x,hh-y);//currently selected vertex
	glEnd();
	glFlush();
	
	
}

//The next 4 functions are for drawing a CIRCLE
void draw_pixel(GLint cx,GLint cy)
{
	
	glBegin(GL_POINTS);
	glVertex2f(cx,cy);
	glEnd();
}
void plotpixels(GLint h,GLint k,GLint x,GLint y)
{
	draw_pixel(x+h,y+k);
	draw_pixel(-x+h,y+k);
	draw_pixel(x+h,-y+k);
	draw_pixel(-x+h,-y+k);
	draw_pixel(y+h,x+k);
	draw_pixel(-y+h,x+k);
	draw_pixel(y+h,-x+k);
	draw_pixel(-y+h,-x+k);
}
void circle_draw(GLint h,GLint k,GLfloat r)
{
	GLint d=1-r,x=0,y=r;
	while(y>x)
	{
		plotpixels(h,k,x,y);
		if(d<0)d+=2*x+3;
		else
		{d+=2*(x-y)+5;--y;}
		++x;
	}
	plotpixels(h,k,x,y);
}
void draw_circle_unfilled(int x,int y)
{
	float temp;//to find the distance between the two points x2,y2,x,y using distance formula from 1st puc maths
	temp=sqrt(double ((x2-x)*(x2-x))+((hh-y2-hh+y)*(hh-y2-hh+y)));
	glColor3f(cr,cg,cb);//draw with the currently selected color
	glPointSize(point_size);
	circle_draw(x2,hh-y2,temp);
}

//function to draw a LINE STRIP
void draw_lines_strip(int x,int y)
{
	glColor3f(cr,cg,cb);//draw with currently selected color
	glLineWidth(line_width);
	glBegin(GL_LINES);
	glVertex2f(x2,hh-y2);//previously selected vertex
	glVertex2f(x,hh-y);//currently selected vertex
	glEnd();
	glFlush();
	x2=x;y2=y;//make the currently selected vertexv as previously selected vertex for the next iteration.
}


//function to draw a COLOR PALETE
void draw_color_box(float r,float g,float b)
{ 
	//COLOR BOX
	glColor3f(r,g,b);
	glBegin(GL_QUADS);
	glVertex2f(w[0],h);
	glVertex2f(w[0],h-30);	
	glVertex2f(w[1],h-30);
	glVertex2f(w[1],h);
	glEnd();
	//COVER BOX for COLOR BOX in WHITE line with width of 1.5
	glLineWidth(1.5);
	glColor3f(0,0,0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(w[0],h);
	glVertex2f(w[0],h-30);	
	glVertex2f(w[1],h-30);
	glVertex2f(w[1],h);
	glVertex2f(w[0],h);
	glEnd();
	h=h-40;
	
}

void draw_toolbar_items()
{
	int i=0;
	
	glColor3f(0,0,0);//Black Color for the Objects inside the Tool Bar Boxes
	glBegin(GL_POINTS);//Circle Box
	i=0;
	while(i!=362)
	{ 
		glVertex2f(wx*525+10*cos(M_PI*i/180),hx*435+10*sin(M_PI*i/180));
		i=i+1;
	}
	glEnd();

	glBegin(GL_LINE_STRIP);//Rectangle Box
	glVertex2f(wx*555,hx*425);glVertex2f(wx*575,hx*425);glVertex2f(wx*575,hx*445);glVertex2f(wx*555,hx*445);glVertex2f(wx*555,hx*425);
	glEnd();

	glBegin(GL_LINES);//Lines Box
	glVertex2f(wx*535,hx*544);glVertex2f(wx*512,hx*564);
	glEnd();

	glBegin(GL_LINE_STRIP);//Lines Strip Box
	glVertex2f(wx*552,hx*543);glVertex2f(wx*560,hx*563);glVertex2f(wx*570,hx*555);glVertex2f(wx*576,hx*570);
	glEnd();

	glBegin(GL_LINE_STRIP);//Pencil Box
	glVertex2f(wx*520,hx*505);glVertex2f(wx*530,hx*505);glVertex2f(wx*530,hx*520);glVertex2f(wx*520,hx*520);glVertex2f(wx*520,hx*505);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(wx*525,hx*505);glVertex2f(wx*525,hx*520);glVertex2f(wx*525,hx*505);glVertex2f(wx*525,hx*520);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(wx*530,hx*520);glVertex2f(wx*520,hx*520);glVertex2f(wx*525,hx*525);
	glEnd();

	glRasterPos2f(wx*555,hx*505);//Text Box
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'A');

	glColor3f(1,1,1);
	glBegin(GL_QUADS);//Eraser Box
	glVertex2f(wx*557.5,hx*467.5);glVertex2f(wx*572.5,hx*467.5);glVertex2f(wx*572.5,hx*482.5);glVertex2f(wx*557.5,hx*482.5);
	glEnd();
	glColor3f(0,0,0);
	glRasterPos2f(wx*561,hx*471);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,'E');

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(wx*525,hx*475);
	glVertex2f(wx*540,hx*475);
	for(int i=0;i<361;i++)
	{
		glColor4f(0,0,0,0);
		glVertex2f(wx*525+15*cos(M_PI*i/180),hx*475+15*sin(M_PI*i/180));
	}
	glEnd();
	glLineWidth(2);
	glColor3f(1,1,1);//White color for the Cover Boxes in the Tool Bar
	glBegin(GL_LINE_LOOP);//Cover Box for the Circle
	glVertex2f(wx*510,hx*420);glVertex2f(wx*540,hx*420);glVertex2f(wx*540,hx*450);glVertex2f(wx*510,hx*450);
	glEnd();
	glBegin(GL_LINE_LOOP);//Cover Box for the Square
	glVertex2f(wx*550,hx*420);glVertex2f(wx*580,hx*420);glVertex2f(wx*580,hx*450);glVertex2f(wx*550,hx*450);
	glEnd();

	glBegin(GL_LINE_LOOP);//Cover Box for the Brush
	glVertex2f(wx*510,hx*460);glVertex2f(wx*540,hx*460);glVertex2f(wx*540,hx*490);glVertex2f(wx*510,hx*490);
	glEnd();
	glBegin(GL_LINE_LOOP);//Cover Box for the Eraser
	glVertex2f(wx*550,hx*460);glVertex2f(wx*580,hx*460);glVertex2f(wx*580,hx*490);glVertex2f(wx*550,hx*490);
	glEnd();

	glBegin(GL_LINE_LOOP);//Cover Box for the Pencil	
	glVertex2f(wx*510,hx*500);glVertex2f(wx*540,hx*500);glVertex2f(wx*540,hx*530);glVertex2f(wx*510,hx*530);
	glEnd();
	glBegin(GL_LINE_LOOP);//Cover Box for the Text		
	glVertex2f(wx*550,hx*500);glVertex2f(wx*580,hx*500);glVertex2f(wx*580,hx*530);glVertex2f(wx*550,hx*530);
	glEnd();

	glBegin(GL_LINE_LOOP);//Cover Box for the Lines
	glVertex2f(wx*510,hx*540);glVertex2f(wx*540,hx*540);glVertex2f(wx*540,hx*570);glVertex2f(wx*510,hx*570);
	glEnd();
	glBegin(GL_LINE_LOOP);//Cover Box for the Lines Strip
	glVertex2f(wx*550,hx*540);glVertex2f(wx*580,hx*540);glVertex2f(wx*580,hx*570);glVertex2f(wx*550,hx*570);
	glEnd();
}
void draw_text(int c)
{
	glColor3f(cr,cg,cb);
	if(typing_check_flag>0)
	{	glRasterPos2f(typing_check_flag+x2+1,hh-y2);typing_check_flag+=20;printf("\nxt=%d,yt=%d",x2,y2);}
	else
	{	glRasterPos2f(x2+1,hh-y2);typing_check_flag+=20;}
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,c);
	glFlush();
}
void draw_rectangel()
{
	glColor3f(cr,cg,cb);
	glLineWidth(line_width);
	glBegin(GL_LINE_STRIP);
	glVertex2f(rx[0],hh-ry[0]);
	glVertex2f(rx[1],hh-ry[1]);
	glVertex2f(rx[2],hh-ry[2]);
	glVertex2f(rx[3],hh-ry[3]);
	glVertex2f(rx[0],hh-ry[0]);
	glEnd();
}

//These 2 functions used for PICKING operations
void drawobjects(GLenum mode)
{
	if(mode==GL_SELECT)glLoadName(1);
	w[0]=wx*510;w[1]=wx*540;h=hx*400;
	draw_color_box(1,1,1);
	if(mode==GL_SELECT)glLoadName(2);
	draw_color_box(0,0,0);
	if(mode==GL_SELECT)glLoadName(3);draw_color_box(1,0,0);
	if(mode==GL_SELECT)glLoadName(4);draw_color_box(0,1,0);
	if(mode==GL_SELECT)glLoadName(5);draw_color_box(0,0,1);
	if(mode==GL_SELECT)glLoadName(6);w[0]=wx*550;w[1]=wx*580;h=hx*400;draw_color_box(1,1,0);
	if(mode==GL_SELECT)glLoadName(7);draw_color_box(0,1,1);
	if(mode==GL_SELECT)glLoadName(8);draw_color_box(1,0,1);
	if(mode==GL_SELECT)glLoadName(9);draw_color_box(0,.7,.5);
	if(mode==GL_SELECT)glLoadName(10);draw_color_box(1,.5,0);
	if(mode==GL_SELECT)glLoadName(11);
	glLineWidth(2);
	glColor4f(1,1,1,0);//White color for the Cover Boxes in the Tool Bar
	/*I made this Color as Transperent as the ALPHA value is Zero,Because We want to select the
	TOOLBAR items for drawing. to select these items i put a invisible box on each item in TOOLBAR 
	so the user can only visible TOOLBAR items.*/
	//So,All the Boxes that are drawn below is invisible.
	glBegin(GL_QUADS);//Cover Box for the Circle
	glVertex2f(wx*510,hx*420);glVertex2f(wx*540,hx*420);glVertex2f(wx*540,hx*450);glVertex2f(wx*510,hx*450);
	glEnd();
	if(mode==GL_SELECT)glLoadName(12);
	glBegin(GL_QUADS);//Cover Box for the Square
	glVertex2f(wx*550,hx*420);glVertex2f(wx*580,hx*420);glVertex2f(wx*580,hx*450);glVertex2f(wx*550,hx*450);
	glEnd();
	if(mode==GL_SELECT)glLoadName(13);
	glBegin(GL_QUADS);//Cover Box for the Brush
	glVertex2f(wx*510,hx*460);glVertex2f(wx*540,hx*460);glVertex2f(wx*540,hx*490);glVertex2f(wx*510,hx*490);
	glEnd();
	if(mode==GL_SELECT)glLoadName(14);
	glBegin(GL_QUADS);//Cover Box for the Eraser
	glVertex2f(wx*550,hx*460);glVertex2f(wx*580,hx*460);glVertex2f(wx*580,hx*490);glVertex2f(wx*550,hx*490);
	glEnd();
   if(mode==GL_SELECT)glLoadName(15);
	glBegin(GL_QUADS);//Cover Box for the Pencil
	glVertex2f(wx*510,hx*500);glVertex2f(wx*540,hx*500);glVertex2f(wx*540,hx*530);glVertex2f(wx*510,hx*530);
	glEnd();
	if(mode==GL_SELECT)glLoadName(16);
	glBegin(GL_QUADS);//Cover Box for the Text
	glVertex2f(wx*550,hx*500);glVertex2f(wx*580,hx*500);glVertex2f(wx*580,hx*530);glVertex2f(wx*550,hx*530);
	glEnd();
	if(mode==GL_SELECT)glLoadName(17);
	glBegin(GL_QUADS);//Cover Box for the Lines
	glVertex2f(wx*510,hx*540);glVertex2f(wx*540,hx*540);glVertex2f(wx*540,hx*570);glVertex2f(wx*510,hx*570);
	glEnd();
	if(mode==GL_SELECT)glLoadName(18);
	glBegin(GL_QUADS);//Cover Box for the Lines Strip
	glVertex2f(wx*550,hx*540);glVertex2f(wx*580,hx*540);glVertex2f(wx*580,hx*570);glVertex2f(wx*550,hx*570);
	glEnd();
	
}
void processhits(GLint hits,GLuint buffer[])
{
	unsigned int i,j;
	GLuint ii,jj,names,*ptr;
	
	ptr=(GLuint *)buffer;
	for(i=0;i<hits;i++)
	{
		names=*ptr;
		ptr+=3;
		for(j=0;j<names;j++)
		{
			switch(*ptr)
			{
			case 1:cr=1;cg=1;cb=1;break;
			case 2:cr=0;cg=0;cb=0;break;
			case 3:cr=1;cg=0;cb=0;break;
			case 4:cr=0;cg=1;cb=0;break;
			case 5:cr=0;cg=0;cb=1;break;
			case 6:cr=1;cg=1;cb=0;break;
			case 7:cr=0;cg=1;cb=1;break;
			case 8:cr=1;cg=0;cb=1;break;
			case 9:cr=0;cg=.7;cb=.5;break;
			case 10:cr=1;cg=.5;cb=0;break;
			case 11:func=4;break;
			case 12:func=6;break;
	     	case 13:func=5;break;
			case 14:func=8;break;
			case 15:func=3;break;
			case 16:func=7;break;
			case 17:func=1;break;
			case 18:func=2;first=1;break;
			}
			ptr++;

	glColor3f(cr,cg,cb);
	glBegin(GL_QUADS);//Current Selected Color Indicator Box in TOOLBAR
	glVertex2f(wx*510,hx*190);
	glVertex2f(wx*580,hx*190);
	glVertex2f(wx*580,hx*120);
	glVertex2f(wx*510,hx*120);
	glEnd();
	glFlush();
		}
	}

}

//Main diplay function
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(.5,.5,.5);
	glBegin(GL_QUADS);//Gray colored TOOLBAR Background Rectangle
	glVertex2f(wx*500,0);
	glVertex2f(wx*600,0);
	glVertex2f(wx*600,hx*600);
	glVertex2f(wx*500,hx*600);
	glEnd();
	glColor3f(.4,0.4,0.4);
	glLineWidth(6);
	glBegin(GL_LINES);//Gray Colored Lines that seperates WORK AREA and TOOLBAR
	glVertex2f(wx*500,0);
	glVertex2f(wx*500,hx*600);
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(wx*500,hx*200);
	glVertex2f(wx*600,hx*200);
	glVertex2f(wx*500,hx*410);
	glVertex2f(wx*600,hx*410);
	glEnd();
	glColor3f(cr,cg,cb);
	glBegin(GL_QUADS);//Current Selected Color Indicator Box in TOOLBAR
	glVertex2f(wx*510,hx*190);
	glVertex2f(wx*580,hx*190);
	glVertex2f(wx*580,hx*120);
	glVertex2f(wx*510,hx*120);
	glEnd();
	glLineWidth(1);
	draw_toolbar_items();
	drawobjects(GL_RENDER);
	glFlush();

}

#define SIZE 512
void motion(int x,int y)
{  
	if(x<wx*500)
	{
	if(func==3)
	{   glColor3f(cr,cg,cb);
	    glPointSize(point_size);
		glBegin(GL_POINTS);
		glVertex2f(x,hh-y);
		glEnd();
		glFlush();
	}
	else if(func==5)
	{ 
		glPointSize(1);
		a=rand()%100;//for the number of points
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(cr,cg,cb,.8);
		glVertex2f(x,hh-y);
		glVertex2f(x+5,hh-y);
		for(int i=0;i<361;i++)
	   {
		glColor4f(cr,cg,cb,0);
		glVertex2f(x+(brush_size/2)*cos(M_PI*i/180),hh-y+(brush_size/2)*sin(M_PI*i/180));
	   }
		glEnd();
		glFlush();
	}
	else if(func==9)	{
		glPointSize(1);
		a=rand()%5;//for the number of points
		
		
		glBegin(GL_POINTS);a=rand()%75;
		glColor3f(cr,cg,cb);
		glVertex2f(x,hh-y);
		glVertex2f(x+5,hh-y);
		for(int i=0;i<a;i++)
	   {
		   glColor4f(cr,cg,cb,.5);
		b=rand()%360;//the angle of each point
		c=rand()%brush_size;//brush size
		glVertex2f(x+c*cos(M_PI*b/180),hh-y+c*sin(M_PI*b/180));
	   }
		glEnd();
		glFlush();
	}

	else if(func==8)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glTranslatef(x-100*eraser_size,hh-y-100*eraser_size,0);
		glScalef(eraser_size,eraser_size,0);
		glCallList(ERASER);
		glPopMatrix();
		glFlush();
	}
	}
}
void Mymenu(int id)
{
	switch(id)
	{
	case 1:brush_size++;
		break;
	case 2:brush_size--;break;
	case 3:line_width++;break;
	case 4:line_width--;break;
	case 5:exit(0);

	case 7:eraser_size+=.1;break;
	case 8:eraser_size-=.1;break;
	case 9:point_size++;break;
	case 10:point_size--;break;
	}
}
void mouse(int button,int status,int x,int y)
{
	if(x<(wx*500))
	{
	if(button==GLUT_LEFT_BUTTON && status==GLUT_DOWN)
	{
				
		switch(func)
		{
		case 1:	//code for drawing a LINE
			/*The Logic is simple,
			if countt=0 store the co-ordinate values and don't draw the line and make count=1 to draw the line on next click
			if count=1 draw the line and make count=0 for to draw a new line*/

			if(countt==0) {x2=x;y2=y;countt=1;}//previosly selected vertex

			else          {draw_line(x,y);countt=0;}/*currently selected vertex.Draw line with the previously selected vertex 
			                                          and currently selected vertex*/
			break;

		case 2://code for drawing a LINE STRIP 
			/*After Selecting this operation(function),
			when the user clicks first time  on the WORK AREA,that initial vertex is stored.
			whenever he clicks next time this vertex is stored(currently seleted vertex).
			the line is drawn between initial vertex and currently seleted vertex.This becomes (previously seleted vertex)
			for the next iteration.
			the line is drawn between previously selected vertex and currently selected vertex
			repeatedly as the user inputs.
			*/
			if(first==1)
			{    //this line is executed only once
				x2=x;y2=y;first=0;//storing initial vertex
			}
			else
			{
				draw_lines_strip(x,y);
			}  
			   	break;

		case 4:	//code for drawing a CIRCLE
			/*
			the logic is same as for drawing a line.only change is instead of drawing a LINE we drawing a CIRCLE
			*/
			if(countt==0)   {x2=x;y2=y;countt=1;}//previously selected vertex

			else         {draw_circle_unfilled(x,y);countt=0;}/*currently selected vertex.find the distance between
															  previously selected vertex and currently selected vertex.
															  the result is the radius.draw the CIRCLE with the obtained
															  radius.
															  */
			break;
		case 6:
			if(rx[3]==-1 && ry[3]==-1 && countt<4)
			{
				rx[countt]=x;ry[countt]=y;
			countt++;
			}
			else
			{
				
					ry[1]=ry[0];
					rx[2]=rx[1];
					rx[3]=rx[0];
					ry[3]=ry[2];
				

				
				/*{
					ry[1]=ry[0];
					rx[2]=rx[1];
					ry[3]=ry[2];
					rx[2]=rx[0];
				}*/
                draw_rectangel();
				for(countt=0;countt<4;countt++)
					rx[countt]=ry[countt]=-1;
				countt=0;
			}
			

			break;

		
		default:
		x2=x;y2=y;typing_check_flag=0;countt=0;break;
	    }
	}
	}
	//this part is reserved for PICKING operations
		GLuint selectbuf[SIZE];
	    GLuint hits;
	    GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glSelectBuffer(SIZE,selectbuf);
		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((GLdouble)x,(GLdouble)(viewport[3]-y),5,5,viewport);
		glOrtho( 0.0,( GLdouble) ww, 0.0, ( GLdouble) hh, - 1.0, 1.0);
		drawobjects(GL_SELECT);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		hits=glRenderMode(GL_RENDER);
		processhits(hits,selectbuf);
		glFlush();



	
}

void reshape(int w,int h)
{ 
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0, ( GLdouble) w, 0.0, ( GLdouble) h, - 1.0, 1.0) ;
    glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,w,h);
	glFlush();
	ww=w;
	hh=h;
	wx=w/600.0;
	hx=h/600.0;
	glNewList(TOOLBAR,GL_COMPILE);
    draw_toolbar_items();
    glEndList();
	glutPostRedisplay();
	
	
	
	}
void keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 27:exit(0);break;
	default:draw_text(key);		
	}
}

void eraser_display_list()
{
	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	glVertex2i(100,100);
	glVertex2i(150,100);
	glVertex2i(150,150);
	glVertex2i(100,150);
	glEnd();
}
int main(int argc,char * argv[])
{ 
int id1,id2,id3,id4,id5;
wx=ww/600;hx=hh/600;
glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
glutInitWindowSize(600,600);
glutCreateWindow("Graphics Editor");
init();
id2=glutCreateMenu(Mymenu);
glutAddMenuEntry("Increase",1);
glutAddMenuEntry("Decrease",2);
id3=glutCreateMenu(Mymenu);
glutAddMenuEntry("Increase",3);
glutAddMenuEntry("Decrease",4);
id4=glutCreateMenu(Mymenu);
glutAddMenuEntry("Increase",7);
glutAddMenuEntry("Decrease",8);
id5=glutCreateMenu(Mymenu);
glutAddMenuEntry("Increase",9);
glutAddMenuEntry("Decrease",10);

id1=glutCreateMenu(Mymenu);
glutAddSubMenu("Brush Size",id2);
glutAddSubMenu("Line Width",id3);
glutAddSubMenu("Eraser Size",id4);
glutAddSubMenu("Point Size",id5);
glutAddMenuEntry("Quit",5);
glutAttachMenu(GLUT_RIGHT_BUTTON);

glutReshapeFunc(reshape);
glutDisplayFunc(display);
glutMouseFunc(mouse);

glutKeyboardFunc(keyboard);
glutMotionFunc(motion);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
glNewList(ERASER,GL_COMPILE);
eraser_display_list();
glEndList();

glutMainLoop();
return 0;


}
