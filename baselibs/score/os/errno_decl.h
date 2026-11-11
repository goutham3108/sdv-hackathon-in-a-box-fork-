/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_OS_ERRNO_DECL_H
#define SCORE_LIB_OS_ERRNO_DECL_H

/*
 * @file errno_decl.h
 * @brief This file has to be included in other files wherever circular dependency of class Error occurs *
 * @attention File is created to adhere to autosar_cpp14_m3_2_3 ODR Rule.
 * ODR Rule : A type, object or function that is used in multiple translation units shall be declared in one and only
 * one file.
 */
namespace score
{
namespace os
{
// Forward declaration of Error class is used to avoid circular dependency issue
class Error;

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_ERRNO_DECL_H
