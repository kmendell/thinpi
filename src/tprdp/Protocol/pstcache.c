

#include "../tprdp.h"

#define MAX_CELL_SIZE 0x1000 /* pixels */

#define IS_PERSISTENT(id) (id < 8 && g_pstcache_fd[id] > 0)

extern int tpServerDepth;
extern RD_BOOL tpBitmapCache;
extern RD_BOOL tpBitmapCachePersistEnable;
extern RD_BOOL tpBitmapCachePrecache;

int g_pstcache_fd[8];
int g_pstcache_Bpp;
RD_BOOL g_pstcache_enumerated = False;
uint8 zero_key[] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Update mru stamp/index for a bitmap */
void pstcache_touch_bitmap(uint8 cache_id, uint16 cache_idx, uint32 stamp)
{
	int fd;

	if (!IS_PERSISTENT(cache_id) || cache_idx >= BMPCACHE2_NUM_PSTCELLS)
		return;

	fd = g_pstcache_fd[cache_id];
	rd_lseek_file(fd, 12 + cache_idx * (g_pstcache_Bpp * MAX_CELL_SIZE + sizeof(CELLHEADER)));
	rd_write_file(fd, &stamp, sizeof(stamp));
}

/* Load a bitmap from the persistent cache */
RD_BOOL
pstcache_load_bitmap(uint8 cache_id, uint16 cache_idx)
{
	uint8 *celldata;
	int fd;
	CELLHEADER cellhdr;
	RD_HBITMAP bitmap;

	if (!tpBitmapCachePersistEnable)
		return False;

	if (!IS_PERSISTENT(cache_id) || cache_idx >= BMPCACHE2_NUM_PSTCELLS)
		return False;

	fd = g_pstcache_fd[cache_id];
	rd_lseek_file(fd, cache_idx * (g_pstcache_Bpp * MAX_CELL_SIZE + sizeof(CELLHEADER)));
	rd_read_file(fd, &cellhdr, sizeof(CELLHEADER));
	celldata = (uint8 *)xmalloc(cellhdr.length);
	rd_read_file(fd, celldata, cellhdr.length);

	bitmap = ui_create_bitmap(cellhdr.width, cellhdr.height, celldata);
	logger(Core, Debug, "pstcache_load_bitmap(), load bitmap from disk: id=%d, idx=%d, bmp=%p)",
		   cache_id, cache_idx, bitmap);
	cache_put_bitmap(cache_id, cache_idx, bitmap);

	xfree(celldata);
	return True;
}

/* Store a bitmap in the persistent cache */
RD_BOOL
pstcache_save_bitmap(uint8 cache_id, uint16 cache_idx, uint8 *key,
					 uint8 width, uint8 height, uint16 length, uint8 *data)
{
	int fd;
	CELLHEADER cellhdr;

	if (!IS_PERSISTENT(cache_id) || cache_idx >= BMPCACHE2_NUM_PSTCELLS)
		return False;

	memcpy(cellhdr.key, key, sizeof(HASH_KEY));
	cellhdr.width = width;
	cellhdr.height = height;
	cellhdr.length = length;
	cellhdr.stamp = 0;

	fd = g_pstcache_fd[cache_id];
	rd_lseek_file(fd, cache_idx * (g_pstcache_Bpp * MAX_CELL_SIZE + sizeof(CELLHEADER)));
	rd_write_file(fd, &cellhdr, sizeof(CELLHEADER));
	rd_write_file(fd, data, length);

	return True;
}

/* List the bitmap keys from the persistent cache file */
int pstcache_enumerate(uint8 id, HASH_KEY *keylist)
{
	int fd, n;
	uint16 idx;
	sint16 mru_idx[0xa00];
	uint32 mru_stamp[0xa00];
	CELLHEADER cellhdr;

	if (!(tpBitmapCache && tpBitmapCachePersistEnable && IS_PERSISTENT(id)))
		return 0;

	/* The server disconnects if the bitmap cache content is sent more than once */
	if (g_pstcache_enumerated)
		return 0;

	logger(Core, Debug, "pstcache_enumerate(), start enumeration");
	for (idx = 0; idx < BMPCACHE2_NUM_PSTCELLS; idx++)
	{
		fd = g_pstcache_fd[id];
		rd_lseek_file(fd, idx * (g_pstcache_Bpp * MAX_CELL_SIZE + sizeof(CELLHEADER)));
		if (rd_read_file(fd, &cellhdr, sizeof(CELLHEADER)) <= 0)
			break;

		if (memcmp(cellhdr.key, zero_key, sizeof(HASH_KEY)) != 0)
		{
			memcpy(keylist[idx], cellhdr.key, sizeof(HASH_KEY));

			/* Pre-cache (not possible for 8-bit colour depth cause it needs a colourmap) */
			if (tpBitmapCachePrecache && cellhdr.stamp && tpServerDepth > 8)
				pstcache_load_bitmap(id, idx);

			/* Sort by stamp */
			for (n = idx; n > 0 && cellhdr.stamp < mru_stamp[n - 1]; n--)
			{
				mru_idx[n] = mru_idx[n - 1];
				mru_stamp[n] = mru_stamp[n - 1];
			}

			mru_idx[n] = idx;
			mru_stamp[n] = cellhdr.stamp;
		}
		else
		{
			break;
		}
	}

	logger(Core, Debug, "pstcache_enumerate(), %d cached bitmaps", idx);

	cache_rebuild_bmpcache_linked_list(id, mru_idx, idx);
	g_pstcache_enumerated = True;
	return idx;
}

/* initialise the persistent bitmap cache */
RD_BOOL
pstcache_init(uint8 cache_id)
{
	int fd;
	char filename[256];

	if (g_pstcache_enumerated)
		return True;

	g_pstcache_fd[cache_id] = 0;

	if (!(tpBitmapCache && tpBitmapCachePersistEnable))
		return False;

	if (!rd_pstcache_mkdir())
	{
		logger(Core, Error,
			   "pstcache_init(), failed to get/make cache directory, disabling feature");
		return False;
	}

	g_pstcache_Bpp = (tpServerDepth + 7) / 8;
	sprintf(filename, "cache/pstcache_%d_%d", cache_id, g_pstcache_Bpp);
	logger(Core, Debug, "pstcache_init(), bitmap cache file %s", filename);

	fd = rd_open_file(filename);
	if (fd == -1)
		return False;

	if (!rd_lock_file(fd, 0, 0))
	{
		logger(Core, Error,
			   "pstcache_init(), failed to lock persistent cache file, disabling feature");
		rd_close_file(fd);
		return False;
	}

	g_pstcache_fd[cache_id] = fd;
	return True;
}
