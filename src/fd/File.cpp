/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 14:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/18 15:41:45 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"

#include <fcntl.h>
#include <unistd.h>

File::File(const std::string& filepath, FDState mode)
    : eof_(false),
      filepath_(filepath)
{
    switch (mode) {
        case S_READ:
            fd_ = open(TMPFILE_NAME, O_RDONLY);
            break;
        case S_WRITE:
            fd_ = open(TMPFILE_NAME, O_CREAT | O_TRUNC | O_WRONLY);
            break;
    }

    if (fd() < 0) {
        throw Exception("Could not create file");
    }
}

FDType File::type() const
{
    return FD_FILE;
}

const std::string& File::filepath() const
{
    return filepath_;
}

bool File::write_done() const
{
    return write_buf_.size() == 0;
}

bool File::read_done() const
{
    return eof_;
}

int File::handle()
{
    ssize_t n = 0;
    switch (state()) {
        case S_WRITE: {
            if (write_buf_.size() == 0) {
                return 0;
            }

            n = write(fd(), write_buf_.data(), write_buf_.size());

            if (n < 0) {
                throw Exception("Error writing to file");
            }

            write_buf_.erase(write_buf_.begin(), write_buf_.begin() + n);
        } break;
        case S_READ: {
            static const std::size_t buf_size = 4096;

            char buf[buf_size];
            n = read(fd(), buf, buf_size);

            if (n < 0) {
                throw Exception("Error reading file");
            }

            if ((std::size_t)n < buf_size) {
                eof_ = true;
            }

            read_buf_.insert(read_buf_.end(), buf, buf + n);
        } break;
    }

    return n;
}
