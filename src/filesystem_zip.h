/*
* This file is part of EasyRPG Player.
*
* EasyRPG Player is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* EasyRPG Player is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _EASYRPG_PLAYER_FILESYSTEM_ZIP_H_
#define _EASYRPG_PLAYER_FILESYSTEM_ZIP_H_

#include "filesystem.h"
#include <fstream>
#include <unordered_map>

class ZIPFilesystem : public Filesystem {
public:

	/**
	* Initializes a  OS Filesystem inside the given ZIP File
	* If you don't know the encoding, or want toknow which sub path contains a certain file
	* use the static function CheckIfContains
	*/
	ZIPFilesystem(std::string const & os_path, std::string const & sub_path, std::string const & encoding);

	/**
	 * Returns the path of the checked file or an empty string if not contained.
	 */
	static bool CheckIfContains(std::string const & os_path, std::string const & filename, std::string & sub_path, std::string & encoding);

	~ZIPFilesystem();
	/**
	* Checks whether the passed path is a file
	*
	* @param path a path relative to the filesystems root
	*/
	bool IsFile(std::string const & path) const override;

	/**
	* Checks whether the passed path is a directory
	*
	* @param path a path relative to the filesystems root
	*/
	bool IsDirectory(std::string const & path) const override;

	/**
	* Checks whether the passed path is an existant file
	*
	* @param path a path relative to the filesystems root
	*/
	bool Exists(std::string const & path) const override;

	/**
	* Retrieves the size of the file on the given path
	*
	* @param path a path relative to the filesystems root
	*/
	uint32_t GetFilesize(std::string const & path) const override;

	/**
	* Allocates a streambuffer with input capabilities on the given path.
	* @param path a path relative to the filesystems root
	* @return A valid pointer to a streambuffer or a nullptr in case of failure.
	*/
	std::streambuf * CreateInputStreambuffer(std::string const & path, int mode)override;

	/**
	* Allocates a streambuffer with output capabilities on the given path.
	* @param path a path relative to the filesystems root
	* @return A valid pointer to a streambuffer or a nullptr in case of failure.
	*/
	std::streambuf * CreateOutputStreambuffer(std::string const & path, int mode) override;

	bool ListDirectoryEntries(std::string const& path, ListDirectoryEntriesCallback callback) const;

private:
	enum class StorageMethod {Unknown,Plain,Deflate};
	struct ZipEntry {
		uint32_t filesize;
		uint32_t fileoffset;
		bool isDirectory;
	};

	struct StreamPoolEntry {
		std::filebuf* filebuffer;
		bool used;
	};
	class StorageIStreambuf;
	class DeflateIStreambuf;
	//No standard Constructor
	ZIPFilesystem() {};

	static bool FindCentralDirectory(std::istream & stream, uint32_t & offset, uint32_t & size, uint16_t & numberOfEntries);
	static bool ReadCentralDirectoryEntry(std::istream & zipfile, std::vector<char> & filepath, uint32_t & offset, uint32_t & uncompressed_size);
	static bool ReadLocalHeader(std::istream & zipfile, uint32_t & offset, StorageMethod & method,uint32_t & compressedSize);

	bool m_isValid;
	std::string m_OSPath;
	std::vector<StreamPoolEntry> m_InputPool;
	std::unordered_map<std::string, ZipEntry> m_zipContent;

};


#endif