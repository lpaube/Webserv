/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/18 14:02:46 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/18 16:00:28 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "FileDescriptor.hpp"
#include <vector>

#define TMPFILE_NAME "tmpfile.tmp"

class File : public FileDescriptor
{
public:
    File(const std::string& filepath, FDState mode);

    virtual FDType type() const;

    const std::string& filepath() const;
    bool write_done() const;
    bool read_done() const;
    int handle();

private:
    std::vector<char> write_buf_;
    std::vector<char> read_buf_;
    bool eof_;
    std::string filepath_;
};
