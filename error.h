// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#pragma once

#include <string>

struct Error final
{
	constexpr Error() noexcept = default;
	constexpr Error(std::string_view what) noexcept : what_{ what } {}

	Error(const Error&) = default;
	Error(Error&&) = default;
	Error& operator=(const Error&) = default;
	Error& operator=(Error&&) = default;

	std::string what_{};
};
