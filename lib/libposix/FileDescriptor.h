/*
 * Copyright (C) 2015 Niek Linnenbank
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBPOSIX_FILEDESCRIPTOR_H
#define __LIBPOSIX_FILEDESCRIPTOR_H

#include <Types.h>
#include <Macros.h>
#include <String.h>

/** Maximum number of open file descriptors. */
#define FILE_DESCRIPTOR_MAX 1024

/**
 * Abstracts a file which is opened by a user process.
 */
class FileDescriptor
{
  public:

    FileDescriptor()
    {
        mount = 0;
        path  = ZERO;
        position = 0;
        open  = false;
    }

    FileDescriptor(const FileDescriptor & fd)
    {
        mount = fd.mount;
        path  = new String(*fd.path);
        position = fd.position;
        open = fd.open;
    }

    // TODO: please modify libstd's Array/Sequence, such that I do not
    // need to implement these. I prefer to just specify a struct.

    bool operator == (const FileDescriptor & fd) const
    {
        return fd.mount == mount &&
              (!fd.path || fd.path->compareTo(*path, true) == 0);
    }

    bool operator != (const FileDescriptor & fd) const
    {
        return !(fd.mount != mount &&
                (!fd.path || fd.path->compareTo(*path, true) == 0));
    }

    /** Filesystem server on which this file was opened. */
    ProcessID mount;

    /** Unique identifier, used by the underlying filesystem. */
    // Address identifier;

    /** Path to the file. */
    String *path;

    /** Current position indicator. */
    Size position;

    /** State of the file descriptor. */
    bool open;
};

#endif /* __LIBPOSIX_FILEDESCRIPTOR_H */
