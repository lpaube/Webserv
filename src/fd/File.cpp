/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 14:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/18 17:11:36 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"

#include <fcntl.h>
#include <unistd.h>

File::File(const std::string& filepath, FDState mode)
    : eof_(false),
      filepath_(filepath),
      w_index_(0),
      error_(false)
{
    state_ = mode;
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
    return state() == S_WRITE && write_buf_.size() == 0;
}

bool File::read_done() const
{
    return state() == S_READ && eof_;
}

void File::append_write_data(const std::vector<char>& data)
{
    write_buf_.insert(write_buf_.end(), data.begin(), data.end());
}

const std::vector<char>& File::get_read_data() const
{
    return read_buf_;
}

#define BUFF_SIZE 8192

int File::handle()
{
    ssize_t n = 0;
    switch (state()) {
        case S_WRITE: {
            if (write_buf_.size() == 0) {
                return 0;
            }

            std::size_t size = write_buf_.size() - (std::size_t)w_index_ > BUFF_SIZE
                                   ? BUFF_SIZE
                                   : write_buf_.size() - (std::size_t)w_index_;

            n = write(fd(), &write_buf_[(std::size_t)w_index_], size);

            if (n < 0) {
                error_ = true;
                throw Exception("Error writing to file");
            }

            w_index_ += n;

            if ((std::size_t)w_index_ == write_buf_.size()) {
                write_buf_.clear();
            }
        } break;
        case S_READ: {
            char buf[BUFF_SIZE];
            n = read(fd(), buf, BUFF_SIZE);

            if (n < 0) {
                error_ = true;
                throw Exception("Error reading file");
            }

            if ((std::size_t)n < BUFF_SIZE) {
                eof_ = true;
            }

            read_buf_.insert(read_buf_.end(), buf, buf + n);
        } break;
    }

    return n;
}

bool File::error() const
{
    return error_;
}
