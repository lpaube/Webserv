/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 14:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/18 16:20:21 by mafortin         ###   ########.fr       */
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
            fd_ = open(filepath.c_str(), O_RDONLY, 0777);
            break;
        case S_WRITE:
            fd_ = open(filepath.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0777);
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
