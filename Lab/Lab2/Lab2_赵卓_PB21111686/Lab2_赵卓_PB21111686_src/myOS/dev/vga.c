extern void outb(unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
short cur_line = 0;
short cur_column = 0;
char *vga_init_p = (char *)0xB8000;

void update_cursor(void)
{
	outb(0x3D4, 0x0E);
	outb(0x3D5, ((cur_line * 80 + cur_column) >> 8) & 0xFF);
	outb(0x3D4, 0x0F);
	outb(0x3D5, (cur_line * 80 + cur_column) & 0xFF);
}

short get_cursor_position(void)
{
	unsigned int high8_bit, low8_bit, location;
	outb(0x3D4, 0x0E);
	high8_bit = inb(0x3D5);
	outb(0x3D4, 0x0F);
	low8_bit = inb(0x3D5);
	location = high8_bit << 8 + low8_bit;
	cur_line = location / 80;
	cur_column = location % 80;
}

void clear_screen()
{
	int *p;
	for (int i = 0; i < 0x1000; i += 4)
	{
		p = (int *)(vga_init_p + i);
		*p = 0x0F200F20;
	}
	cur_line = 0;
	cur_column = 0;
	update_cursor();
}

void roll_screen()
{
	int *pread, *pwrite;
	int i = 0;
	for (int i = 0; i < (cur_line * 80 + cur_column); i += 2)
	{
		pwrite = (int *)(vga_init_p + i * 2);
		if (i < (cur_line * 80 + cur_column - 80))
		{
			pread = (int *)(vga_init_p + i * 2 + 160);
			*(pwrite) = *(pread);
		}
		else
			*(pwrite) = 0x0F200F20;
	}
	cur_line--;
	update_cursor();
}

void putsinglechar(unsigned char c, int color)
{
	while (cur_line >= 25)
		roll_screen();
	unsigned char *p;
	p = (unsigned char *)(vga_init_p + (cur_line * 80 + cur_column) * 2);
	*p = c;
	p = (unsigned char *)(vga_init_p + (cur_line * 80 + cur_column) * 2 + 1);
	*p = (unsigned char)color;
	if (cur_column == 80)
	{
		cur_line++;
		cur_column = 0;
	}
	else
		cur_column++;
	update_cursor();
}


void append2screen(char *str, int color)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == '\n')
		{
			cur_line++;
			cur_column = 0;
			update_cursor();
			while (cur_line >= 25)
				roll_screen();
		}
		else
			putsinglechar(str[i], color);
	}
}
