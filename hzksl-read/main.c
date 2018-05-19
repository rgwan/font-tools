#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#define DrawSleep() //Sleep(10)
void dispcharacter(uint8_t Qu, uint8_t Wei, uint8_t mul, uint8_t div)
{
	uint32_t datPos = 0;
	uint16_t datSize = 0;
	uint32_t indexPos = ((Qu - 16) * 94 + Wei - 1) * 6;
	uint8_t cmd;
	uint8_t ctrl;
	uint8_t pos;
	uint8_t subMode;

    int start_x = 0;
    int start_y = 0;
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;


	int i = 0;
	int j;
	FILE *fp = fopen("HZKSLSTF", "rb");

	if(div == 0) div = 1;
	if(mul == 0) mul = 1;
	//FILE *flog = fopen("draw.txt", "w+");

	/* 查到字形数据 */
	fseek(fp, indexPos, SEEK_SET);
	fread(&datPos, sizeof(datPos), 1, fp);
	fread(&datSize, sizeof(datSize), 1, fp);
	printf("indexPos = %x, datPos = %x, datSize = %x\n",
			indexPos, datPos, datSize);
	/* 解码 */
	fseek(fp, datPos, SEEK_SET);
	//fprintf(flog, "开始\n");
	while(i < datSize)
	{
		fread(&cmd, sizeof(cmd), 1, fp);
		i++;
		ctrl = cmd >> 6;
		subMode = cmd & 0x30;
        if(ctrl == 0x03)
		{//起始坐标 11
			fread(&pos, sizeof(pos), 1, fp);
			i++;
			//printf("上次坐标: %d, %d\n", x, y);
			//line(start_x, start_y, x, y);
			line(x, y, start_x, start_y);
			DrawSleep();
            start_x = ((cmd & 0x3f) << 1) + (pos >> 7);
            start_y = pos & 0x7f;
            start_x *= mul; start_x /= div;
            start_y *= mul; start_y /= div;
            x = start_x;
            y = start_y;
            //printf("起始坐标: %d, %d, 控制码 0x%02x\n", x, y, cmd);
		}
		else if(ctrl == 0x00)
		{ // 3bit, 00
			for(j = 0; j < (cmd & 0x3f); j++)
			{
				fread(&pos, sizeof(pos), 1, fp);
				i++;
				dx =  ((pos & 0x80)? -((pos >> 4) & 0x07): ((pos >> 4) & 0x07));
				dy =  ((pos & 0x08)? -(pos & 0x07): (pos & 0x07));
				dx *= mul; dx /= div;
				dy *= mul; dy /= div;
				line(x, y, x + dx , y + dy);
				DrawSleep();
				x = x + dx;
				y = y + dy;
			//	fprintf(flog, "变化坐标: %d, %d, 控制码 0x%02x\n", dx, dy, cmd);
			}
		}
		else if(ctrl == 0x01)
		{ // 4bit 01
			for(j = 0; j < (cmd & 0x0f); j++)
			{
				fread(&pos, sizeof(pos), 1, fp);
				i++;
				dx = (pos >> 4) & 0x0f;
				dy =  pos & 0x0f;
                /* 这是谁设计的,站出来! */
				if(subMode == 0x10)
					dx = -dx;
				else if(subMode == 0x20)
				{
					dx = -dx;
					dy = -dy;
				}
				else if(subMode == 0x30)
					dy = -dy;

				dx *= mul; dx /= div;
				dy *= mul; dy /= div;
				line(x, y, x + dx , y + dy);
				DrawSleep();
				x = x + dx;
				y = y + dy;
			//	fprintf(flog, "变化坐标: %d, %d, 控制码 0x%02x\n", dx, dy, cmd);
			}
		}
		else if(ctrl == 0x02)
		{ // 10
			fread(&pos, sizeof(pos), 1, fp);
			i++;
			if(subMode == 0x00)
			{
				dx = ((cmd & 0x08)? -(cmd & 0x07): (cmd & 0x07));
				dy = ((pos & 0x80)? -(pos & 0x7f): (pos & 0x7f));
				dx *= mul; dx /= div;
				dy *= mul; dy /= div;
				line(x, y, x + dx, y + dy);
				DrawSleep();
				x = x + dx;
				y = y + dy;
			}
			else if(subMode == 0x10)
			{
				dy = ((cmd & 0x08)? -(cmd & 0x07): (cmd & 0x07));
				dx = ((pos & 0x80)? -(pos & 0x7f): (pos & 0x7f));
				dx *= mul; dx /= div;
				dy *= mul; dy /= div;
				line(x, y, x + dx, y + dy);
				DrawSleep();
				x = x + dx;
				y = y + dy;
			}
			else
			{
				dx = (pos & 0x80)? -(pos & 0x7f): (pos & 0x7f);
				fread(&pos, sizeof(pos), 1, fp);
				i++;
				dy = (pos & 0x80)? -(pos & 0x7f): (pos & 0x7f);
				dx *= mul; dx /= div;
				dy *= mul; dy /= div;
				line(x, y, x + dx, y + dy);
				DrawSleep();
				x = x + dx;
				y = y + dy;
			}
			//fprintf(flog, "变化坐标: %d, %d, 控制码 0x%02x\n", dx, dy, cmd);
		}
	}
	line(x, y, start_x, start_y);//最后补一条
	DrawSleep();
	fclose(fp);
	//fclose(flog);
}

int main()
{
	int i;
	int Qu, Wei;
	initgraph(1024, 768);
	cleardevice();          //图形界面初始化。
	setbkcolor(BLUE);
	setcolor(WHITE);

	printf("Enter Qu Wei code\n");
	scanf("%d %d", &Qu, &Wei);

	for(i = 0; i < 1024; i++)
	{
		//rectangle(0, 0, 96, ); // 96 x 96 正方形
		dispcharacter(Qu, i + Wei, 4, 1);// 阿, + 0x2020 + 0x8080
		getch();
		cleardevice();
	}

	closegraph();
}


