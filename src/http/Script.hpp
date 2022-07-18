/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/07/18 16:17:10 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core/ExceptionBase.hpp"
#include "core/ServerParser.hpp"
#include "http/Request.hpp"

class Script
{
public:
    class Exception : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

    bool ext_found;

public:
    Script(const Config& config, const Request& request);
    ~Script();
    void exec(const std::string& file_name, const int& fd_out);
    std::string get_ext(const std::string& path);

private:
    void build_cmd(const std::string& path, const Config& config);
    void build_env(Method method, const Config& config);
    void print_env() const;

private:
    std::vector<std::string> v_env_;
    char** envp_;
    char** cmd_;
    std::size_t envp_size_;
    const Request& request_;
};
