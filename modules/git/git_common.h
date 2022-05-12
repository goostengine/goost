#pragma once

#include <git2.h>

#include "core/ustring.h"

bool check_errors(int error, String message, String function, String file, int line);

extern "C" int diff_line_callback_function(const git_diff_delta *delta, const git_diff_hunk *hunk, const git_diff_line *line, void *payload);
extern "C" int diff_hunk_cb(const git_diff_delta *delta, const git_diff_hunk *range, void *payload);

#define GIT2_CALL(function_call, m_error_msg)                                       \
	if (check_errors(function_call, m_error_msg, __FUNCTION__, __FILE__, __LINE__)) \
		return;

#define GIT2_CALL_V(function_call, m_error_msg, m_return)                           \
	if (check_errors(function_call, m_error_msg, __FUNCTION__, __FILE__, __LINE__)) \
		return m_return;
