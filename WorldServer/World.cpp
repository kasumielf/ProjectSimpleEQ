#include "World.h"

//#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <png.h>
#include <iostream>

World::World()
{
	for (int i = 0; i < MAX_WORLD_HEIGHT; ++i)
	{
		for (int j = 0; j < MAX_WORLD_WIDTH; ++j)
		{
			world[i][j] = nullptr;
		}
	}
}


World::~World()
{
}

Object * const World::GetObjectById(unsigned int id)
{
	if (objects.count(id) > 0)
	{
		return objects[id];
	}

	return nullptr;
}

void World::WorldInit()
{
	for (int y = 0; y < MAX_WORLD_HEIGHT; ++y)
	{
		for (int x = 0; x < MAX_WORLD_WIDTH; ++x)
		{

		}
	}
}

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

std::array<std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT> World::GetBlockDataFromBitmap(const char * filename)
{
	std::array<std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT> data;

	int x, y;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	char header[8];    // 8 is the maximum size that can be checked
	png_const_bytep d = 0;

	FILE *fp = fopen(filename, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", filename);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_const_bytep)header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", filename);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during read_image");

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

	for (y = 0; y<height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(fp);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			data[j][i] = row_pointers[i][j] == 0 ? true : false;
		}
	}
	return data;
}

void World::BlockCellInit(std::array< std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT > data)
{
	for (int y = 0; y < MAX_WORLD_HEIGHT; ++y)
	{
		for (int x = 0; x < MAX_WORLD_WIDTH; ++x)
		{
			int deg = 0;

			for (int i = 0; i < 8; ++i)
			{
				int v_x = std::round(::sin(deg * 3.14 / 180));
				int v_y = std::round(::cos(deg * 3.14 / 180));

				if (x + v_x >= MAX_WORLD_WIDTH || x + v_x < 0)	v_x = 0;
				if (y + v_y >= MAX_WORLD_HEIGHT || y + v_y < 0)	v_y = 0;

				block[y + v_y][x + v_x][i] = (bool)data[y + v_y][x + v_x];
				deg += 45;
			}
		}
	}
}

void World::CreateObject(Object * obj)
{
	
	unsigned int id = obj->GetId();

	if (objects[id] == nullptr)
	{
		objects[id] = obj;
	}
}

void World::DeleteObject(unsigned int id)
{
	if (objects[id] != nullptr)
	{
		DeleteObject(objects[id]);
	}
}

void World::DeleteObject(Object * obj)
{
	sector[obj->getCurrSectorY()][obj->getCurrSectorX()].RemovePlayer(obj->GetId());
	objects.erase(obj->GetId());
	RemoveObject(obj->GetX(), obj->GetY());

	delete obj;
}

void World::RemoveObject(unsigned short x, unsigned short y)
{
	world[y][x] = nullptr;
}

void World::MoveObject(Object * obj, const char direction)
{
	// 0 : UP 2: RIGHT 4 : DOWN : 6 : LEFT

	short from_x = obj->GetX(), from_y = obj->GetY();
	
	if (block[from_y][from_x][direction] == false)
	{
		obj->MoveByDirection(direction);
		short to_x = obj->GetX(), to_y = obj->GetY();

		if (world[to_y][to_x] == nullptr && world[from_y][from_x] != nullptr)
		{
			Object* ptr = world[from_y][from_x];
			world[from_y][from_x] = nullptr;
			world[to_y][to_x] = ptr;
		}

		SetSector(obj, to_x, to_y);
	}
}

void World::SetSector(Object* obj, unsigned short x, unsigned short y)
{
	if (sector[y / MAX_SECTOR_HEIGHT][x / MAX_SECTOR_WIDTH].GetPlayer(obj->GetId()) == nullptr)
	{
		if (obj->getCurrSectorX() >= 0)
			sector[obj->getCurrSectorY()][obj->getCurrSectorX()].RemovePlayer(obj->GetId());

		sector[y / MAX_SECTOR_HEIGHT][x / MAX_SECTOR_WIDTH].AddPlayer(obj->GetId(), obj);
		obj->setCurSectorPos(x / MAX_SECTOR_HEIGHT, y / MAX_SECTOR_WIDTH);
	}

}
void World::Destroy()
{
}


