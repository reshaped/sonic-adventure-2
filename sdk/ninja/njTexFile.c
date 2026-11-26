/* texture file reading through a sector buffer */
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the member exports these functions and this data, though no header of the
 * SDK declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* read as njTexFread does, but for a read whose bytes past the buffer run to
 * the file's end. the call reads those bytes straight into buf, in whole
 * sectors. buf must have room up to the end of the file's last sector. it
 * returns 1 even where gdFsRead fails to read them. the next byte to read is
 * then the one past the file's last sector */
Sint32 njTexFread2(void *buf, Uint32 size);

/* the buffer the reads go through. _nj_file_buffer_max_size is its size in
 * bytes, and _nj_buffer_sect_size_ its size in sectors */
Sint8 *_nj_file_buffer_st = _nj_file_buffer;
Sint32 _nj_file_buffer_max_size = GDD_FS_SCTSIZE;
Sint32 _nj_buffer_sect_size_ = 1;
/* the open file's state. the initializers put it in the data section, where
 * the image holds it.
 *
 * while _nj_buffer_r_byte_size_ isn't 0, the reads assume the buffer holds
 * the _nj_buffer_sect_size_ sectors from sector _nj_file_sect_ptr_. a read
 * reloads the buffer where its first byte lies outside them, or where
 * _nj_buffer_r_byte_size_ is 0 */
Sint32 _nj_file_sect_ptr_ = 0;
Sint32 _nj_sect_max_size_ = 0; /* the file's size in sectors */
Sint32 _nj_file_max_size_ = 0; /* the file's size in bytes */
Sint32 _nj_file_byte_ptr_ = 0; /* the next byte to read, as a file offset */
Sint32 _nj_buffer_byte_ptr_ = 0; /* the next byte to read, as a buffer offset */
/* the buffer's bytes from _nj_buffer_byte_ptr_ to its end */
Sint32 _nj_buffer_r_byte_size_ = 0;
Sint32 _nj_buffer_read_flag_ = 0;

GDFS _nj_file_ptr; /* the open file */
/* NOLINTEND(misc-use-internal-linkage) */

#pragma inline(record_read_failure)
static void record_read_failure(void)
{
    _nj_texerr_.gdstat = gdFsGetStat(_nj_file_ptr);
    _nj_texerr_.gderr = gdFsGetErrStat(_nj_file_ptr);
    _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
    /* @hack redundant: a label nothing jumps to */
done:; /* NOLINT(clang-diagnostic-unused-label) */
}

Sint32 njTexFopen(const char *filename)
{
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((_nj_file_ptr = gdFsOpen(filename, NULL)) == NULL) {
        _nj_texerr_.texerr = NJD_TEXERR_FILEOPEN;
        return -2;
    }
    if (!gdFsGetFileSize(_nj_file_ptr, &_nj_file_max_size_)) {
        _nj_texerr_.texerr = NJD_TEXERR_FILEOPEN;
        return -2;
    }
    _nj_sect_max_size_ = gdFsCalcSctSize(_nj_file_max_size_);
    _nj_file_sect_ptr_ = 0;
    _nj_file_byte_ptr_ = 0;
    _nj_buffer_byte_ptr_ = 0;
    _nj_buffer_r_byte_size_ = 0;
    _nj_buffer_read_flag_ = 0;
    return 0;
}

void njTexFclose(void)
{
    gdFsClose(_nj_file_ptr);
}

Sint32 njTexFread(void *buf, Uint32 size)
{
    Sint32 sector;
    Sint32 offset;
    Sint32 held;
    Sint32 count;
    Uint32 done;

    if (size == 0) {
        _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
        return -1;
    }
    if ((Uint32)_nj_file_max_size_ < _nj_file_byte_ptr_ + size) {
        _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
        return -1;
    }
    sector = _nj_file_byte_ptr_ / _nj_file_buffer_max_size;
    offset = _nj_file_byte_ptr_ - (sector * _nj_file_buffer_max_size);
    /* @hack spelling: held, a local for the quotient, where the quotient in
     * the test would do */
    held = _nj_file_sect_ptr_ / _nj_buffer_sect_size_;
    if (sector != held || _nj_buffer_r_byte_size_ == 0) {
        sector *= _nj_buffer_sect_size_;
        if (gdFsSeek(_nj_file_ptr, sector, GDD_SEEK_SET)) {
            _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
            return -1;
        }
        if (gdFsRead(_nj_file_ptr, _nj_buffer_sect_size_, _nj_file_buffer_st)) {
            record_read_failure();
            return -1;
        }
        _nj_file_sect_ptr_ = sector;
    }
    /* @hack redundant: the test, which guards a store of the value it
     * compares, where offset stored untested would do */
    if (offset != _nj_buffer_byte_ptr_) {
        _nj_buffer_byte_ptr_ = offset;
    }
    _nj_buffer_r_byte_size_ = _nj_file_buffer_max_size - _nj_buffer_byte_ptr_;
    /* @hack type: the buffer and buf addresses as integer sums, here and
     * below, where pointer sums would do */
    if ((Uint32)_nj_buffer_r_byte_size_ >= size) {
        njMemCopy(buf,
                  (void *)((Uint32)_nj_file_buffer_st + _nj_buffer_byte_ptr_),
                  size);
        _nj_buffer_byte_ptr_ += size;
        _nj_file_byte_ptr_ += size;
        _nj_buffer_r_byte_size_ =
            _nj_file_buffer_max_size - _nj_buffer_byte_ptr_;
    } else {
        done = 0;
        if (_nj_buffer_r_byte_size_ != 0) {
            njMemCopy(
                buf,
                (void *)((Uint32)_nj_file_buffer_st + _nj_buffer_byte_ptr_),
                _nj_buffer_r_byte_size_);
            done = _nj_buffer_r_byte_size_;
            _nj_file_byte_ptr_ += _nj_buffer_r_byte_size_;
        }
        /* @hack spelling: a test, then a do-while, where a while loop would
         * do */
        if (size > done) {
            do {
                if (size - done > (Uint32)_nj_file_buffer_max_size) {
                    count = (size - done) / _nj_file_buffer_max_size;
                    if (gdFsRead(_nj_file_ptr, count * _nj_buffer_sect_size_,
                                 (void *)((Uint32)buf + done))) {
                        record_read_failure();
                        return -1;
                    }
                    done += count * _nj_file_buffer_max_size;
                    _nj_file_byte_ptr_ += count * _nj_file_buffer_max_size;
                    /* @bug logic: after a seek back, a read can copy an older
                     * sector's bytes. where sectors read straight into buf end
                     * the read, _nj_file_sect_ptr_ moves to the last of them,
                     * MOV.L R0,@R8 at 0x8c13a9b0. the buffer keeps the earlier
                     * sector it held, and nothing reloads it. a later read in
                     * that last sector finds the pointer on it and
                     * _nj_buffer_r_byte_size_ above 0, CMP/EQ R4,R11 at
                     * 0x8c13a84a and TST R3,R3 at 0x8c13a852. it copies the
                     * buffer without reloading it */
                    _nj_file_sect_ptr_ += count * _nj_buffer_sect_size_;
                    _nj_buffer_byte_ptr_ = 0;
                } else {
                    count = size - done;
                    if (gdFsRead(_nj_file_ptr, _nj_buffer_sect_size_,
                                 _nj_file_buffer_st)) {
                        record_read_failure();
                        return -1;
                    }
                    njMemCopy((void *)((Uint32)buf + done), _nj_file_buffer_st,
                              count);
                    _nj_file_byte_ptr_ += count;
                    _nj_file_sect_ptr_ += _nj_buffer_sect_size_;
                    _nj_buffer_byte_ptr_ = count;
                    _nj_buffer_r_byte_size_ =
                        _nj_file_buffer_max_size - _nj_buffer_byte_ptr_;
                    done += count;
                }
            } while (size > done);
        }
    }
    return 1;
}

Sint32 njTexFread2(void *buf, Uint32 size)
{
    Sint32 sector;
    Sint32 offset;
    Sint32 held;
    Sint32 count;
    Sint32 bytes;
    Uint32 done;

    if (size == 0) {
        _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
        return -1;
    }
    if ((Uint32)_nj_file_max_size_ < _nj_file_byte_ptr_ + size) {
        _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
        return -1;
    }
    sector = _nj_file_byte_ptr_ / _nj_file_buffer_max_size;
    offset = _nj_file_byte_ptr_ - (sector * _nj_file_buffer_max_size);
    /* @hack spelling: held, a local for the quotient, where the quotient in
     * the test would do */
    held = _nj_file_sect_ptr_ / _nj_buffer_sect_size_;
    if (sector != held || _nj_buffer_r_byte_size_ == 0) {
        sector *= _nj_buffer_sect_size_;
        if (gdFsSeek(_nj_file_ptr, sector, GDD_SEEK_SET)) {
            _nj_texerr_.texerr = NJD_TEXERR_FILELOAD;
            return -1;
        }
        if (gdFsRead(_nj_file_ptr, _nj_buffer_sect_size_, _nj_file_buffer_st)) {
            record_read_failure();
            return -1;
        }
        _nj_file_sect_ptr_ = sector;
    }
    /* @hack redundant: the test, which guards a store of the value it
     * compares, where offset stored untested would do */
    if (offset != _nj_buffer_byte_ptr_) {
        _nj_buffer_byte_ptr_ = offset;
    }
    _nj_buffer_r_byte_size_ = _nj_file_buffer_max_size - _nj_buffer_byte_ptr_;
    /* @hack type: the buffer and buf addresses as integer sums, here and
     * below, where pointer sums would do */
    if ((Uint32)_nj_buffer_r_byte_size_ >= size) {
        njMemCopy(buf,
                  (void *)((Uint32)_nj_file_buffer_st + _nj_buffer_byte_ptr_),
                  size);
        _nj_buffer_byte_ptr_ += size;
        _nj_file_byte_ptr_ += size;
        _nj_buffer_r_byte_size_ =
            _nj_file_buffer_max_size - _nj_buffer_byte_ptr_;
    } else {
        done = 0;
        if (_nj_buffer_r_byte_size_ != 0) {
            njMemCopy(
                buf,
                (void *)((Uint32)_nj_file_buffer_st + _nj_buffer_byte_ptr_),
                _nj_buffer_r_byte_size_);
            done = _nj_buffer_r_byte_size_;
            _nj_file_byte_ptr_ += _nj_buffer_r_byte_size_;
        }
        if (size - done >= (Uint32)(_nj_file_max_size_ - _nj_file_byte_ptr_)) {
            count = gdFsCalcSctSize(size - done);
            gdFsRead(_nj_file_ptr, count, (void *)((Uint32)buf + done));
            /* @hack spelling: bytes, a local for the sectors' bytes, where
             * the product added in place would do */
            bytes = count * GDD_FS_SCTSIZE;
            _nj_file_byte_ptr_ += bytes;
            /* @bug logic: after a seek back to the file's first sector, a
             * read can copy another sector's bytes. _nj_file_sect_ptr_ takes
             * 0, MOV.L R3,@R8 at 0x8c13abb4, though the buffer keeps the
             * sector it held and _nj_buffer_r_byte_size_ stays above 0. a
             * later read in sector 0 finds both so, CMP/EQ R4,R11 at
             * 0x8c13aa82 and TST R3,R3 at 0x8c13aa8a. it copies the buffer
             * without reloading it */
            _nj_file_sect_ptr_ = 0;
            _nj_buffer_byte_ptr_ = 0;
        } else if (size > done) {
            /* @hack spelling: a do-while under a test of its condition,
             * where a while loop in the else would do */
            do {
                if (size - done > (Uint32)_nj_file_buffer_max_size) {
                    count = (size - done) / _nj_file_buffer_max_size;
                    if (gdFsRead(_nj_file_ptr, count * _nj_buffer_sect_size_,
                                 (void *)((Uint32)buf + done))) {
                        record_read_failure();
                        return -1;
                    }
                    done += count * _nj_file_buffer_max_size;
                    _nj_file_byte_ptr_ += count * _nj_file_buffer_max_size;
                    /* @bug logic: after a seek back, a read can copy an older
                     * sector's bytes, as in njTexFread. MOV.L R0,@R8 at
                     * 0x8c13ac28 moves _nj_file_sect_ptr_, and CMP/EQ R4,R11
                     * at 0x8c13aa82 and TST R3,R3 at 0x8c13aa8a skip the
                     * reload */
                    _nj_file_sect_ptr_ += count * _nj_buffer_sect_size_;
                    _nj_buffer_byte_ptr_ = 0;
                } else {
                    count = size - done;
                    if (gdFsRead(_nj_file_ptr, _nj_buffer_sect_size_,
                                 _nj_file_buffer_st)) {
                        record_read_failure();
                        return -1;
                    }
                    njMemCopy((void *)((Uint32)buf + done), _nj_file_buffer_st,
                              count);
                    _nj_file_byte_ptr_ += count;
                    _nj_file_sect_ptr_ += _nj_buffer_sect_size_;
                    _nj_buffer_byte_ptr_ = count;
                    _nj_buffer_r_byte_size_ =
                        _nj_file_buffer_max_size - _nj_buffer_byte_ptr_;
                    done += count;
                }
            } while (size > done);
        }
    }
    return 1;
}

void njTexFseek(Sint32 offset, Sint32 type)
{
    /* @hack spelling: GDD_SEEK_CUR's case first, where the cases in their
     * values' order would do */
    switch (type) {
    case GDD_SEEK_CUR:
        _nj_file_byte_ptr_ += offset;
        break;
    case GDD_SEEK_SET:
        _nj_file_byte_ptr_ = offset;
        break;
    case GDD_SEEK_END:
        _nj_file_byte_ptr_ = _nj_file_max_size_ - offset;
        break;
    }
}
