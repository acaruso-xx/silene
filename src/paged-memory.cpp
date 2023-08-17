#include "paged-memory.hpp"

std::uint8_t* PagedMemory::ptr_to_addr(std::uint32_t vaddr) {
	return _backing_memory + vaddr;
}

std::uint8_t PagedMemory::read_byte(std::uint32_t vaddr) {
	auto ret = this->ptr_to_addr(vaddr);
	return *ret;
}

std::uint16_t PagedMemory::read_halfword(std::uint32_t vaddr) {
	auto ret = reinterpret_cast<std::uint16_t*>(this->ptr_to_addr(vaddr));
	// spdlog::trace("read half: {:#08x} = {:#04x}", vaddr, *ret);
	return *ret;
}

std::uint32_t PagedMemory::read_word(std::uint32_t vaddr) {
	auto ret = reinterpret_cast<std::uint32_t*>(this->ptr_to_addr(vaddr));
	// spdlog::trace("read word: {:#08x} = {:#08x}", vaddr, *ret);
	return *ret;
}

std::uint64_t PagedMemory::read_doubleword(std::uint32_t vaddr) {
	auto ret = reinterpret_cast<std::uint64_t*>(this->ptr_to_addr(vaddr));
	return *ret;
}

void PagedMemory::write_byte(std::uint32_t vaddr, std::uint8_t value) {
	auto ptr = this->ptr_to_addr(vaddr);
	*ptr = value;
}

void PagedMemory::write_halfword(std::uint32_t vaddr, std::uint16_t value) {
	auto ptr = reinterpret_cast<std::uint16_t*>(this->ptr_to_addr(vaddr));
	*ptr = value;
}

void PagedMemory::write_word(std::uint32_t vaddr, std::uint32_t value) {
	auto ptr = reinterpret_cast<std::uint32_t*>(this->ptr_to_addr(vaddr));
	*ptr = value;
}

void PagedMemory::write_doubleword(std::uint32_t vaddr, std::uint64_t value) {
	auto ptr = reinterpret_cast<std::uint64_t*>(this->ptr_to_addr(vaddr));
	*ptr = value;
}

void PagedMemory::allocate_stack(std::uint32_t stack_size) {
	this->_stack_min -= stack_size;
}

void PagedMemory::allocate(std::uint32_t bytes) {
	if (this->_max_addr + bytes > this->_stack_min) {
		spdlog::warn("memory is beginning to overrun the stack!");
	}
	this->_max_addr += bytes;
};

std::uint32_t PagedMemory::get_next_addr() {
	return this->_max_addr;
}

std::uint32_t PagedMemory::get_next_word_addr() {
	if (this->_max_addr & 1) {
		// if the returned pointer is thumbed, increment by 1 to remove it
		this->_max_addr++;
	}

	return this->_max_addr;
}

std::uint32_t PagedMemory::get_next_page_aligned_addr() {
	auto current_page = this->_max_addr / PAGE_SIZE;
	auto offset = this->_max_addr % PAGE_SIZE;

	if (offset == 0) {
		// do not allocate, we ended on a page
		return this->_max_addr;
	}

	auto next_addr = PAGE_SIZE * (current_page + 1);
	this->_max_addr = next_addr;

	return next_addr;
}

void PagedMemory::copy(std::uint32_t vaddr, void* src, std::uint32_t length) {
	std::memcpy(this->_backing_memory + vaddr, src, length);

	if (this->_max_addr < vaddr + length) {
		this->_max_addr = vaddr + length;
	}
}
