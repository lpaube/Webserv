/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationContent.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 22:13:26 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/01 20:15:44 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>

class LocationContent
{
public:
    LocationContent();
    ~LocationContent(){};
    std::string path;
    std::vector<std::string> loc_content;
    std::string loc_content_str;

private:
    void VectorizeLocation();
};
