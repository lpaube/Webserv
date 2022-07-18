/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:08:37 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/18 15:42:22 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core/ExceptionBase.hpp"

enum FDType {
    FD_TCP_LISTENER,
    FD_TCP_CONNECTION,
    FD_FILE
};

enum FDState {
    S_READ,
    S_WRITE
};

class FileDescriptor
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const std::string& msg);
    };

public:
    FileDescriptor();
    virtual ~FileDescriptor();

    virtual FDType type() const = 0;

    int fd() const;
    bool is_init() const;
    FDState state() const;
    void set_state(FDState state);

protected:
    int fd_;
    FDState state_;

private:
    FileDescriptor(const FileDescriptor&);
    FileDescriptor& operator=(const FileDescriptor&);
};
