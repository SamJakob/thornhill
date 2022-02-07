# Originally from: https://www.mattkeeter.com/blog/2018-01-06-versioning/

execute_process(COMMAND git log --pretty=format:'%h' -n 1
        OUTPUT_VARIABLE GIT_REV
        ERROR_QUIET)

# Check whether we got any revision (which isn't
# always the case, e.g. when someone downloaded a zip
# file from Github instead of a checkout)
if ("${GIT_REV}" STREQUAL "")
    set(GIT_REV "N/A")
    set(GIT_DIFF "")
    set(GIT_TAG "N/A")
    set(GIT_BRANCH "N/A")
else()
    execute_process(
            COMMAND bash -c "git diff --quiet --exit-code ':(exclude)../config/version.h' || echo '-WORKING'"
            OUTPUT_VARIABLE GIT_DIFF)
    execute_process(
            COMMAND git describe --exact-match --tags
            OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
    execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
    string(STRIP "${GIT_DIFF}" GIT_DIFF)
    string(STRIP "${GIT_TAG}" GIT_TAG)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

string(TIMESTAMP BUILD_DATE "%m/%d/%Y")
string(TIMESTAMP BUILD_DATE_UTC "%m/%d/%Y" UTC)

string(TIMESTAMP BUILD_TIME "%H:%M:%S")
string(TIMESTAMP BUILD_TIME_UTC "%H:%M:%S" UTC)

set(VERSION "/*
 * Howdy!
 *
 * This file is automatically generated on a new build.
 * You should not change it manually!
 */

#pragma once

#define TH_GIT_REV \"${GIT_REV}${GIT_DIFF}\"
#define TH_GIT_TAG \"${GIT_TAG}\"
#define TH_GIT_BRANCH \"${GIT_BRANCH}\"

#define TH_BUILD_DATE \"${BUILD_DATE}\"
#define TH_BUILD_DATE_UTC \"${BUILD_DATE_UTC}\"
#define TH_BUILD_TIME \"${BUILD_TIME}\"
#define TH_BUILD_TIME_UTC \"${BUILD_TIME_UTC}\"
")

if (NOT DEFINED TH_CURRENT_SOURCE_DIR)
    message(FATAL_ERROR "You must set TH_CURRENT_SOURCE_DIR to the root of the project to run this script.")
endif()

file(WRITE ${TH_CURRENT_SOURCE_DIR}/config/version.h "${VERSION}")
