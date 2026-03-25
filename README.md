# Wikipedia Explorer Filesystem

This is a userspace filesystem that allows accessing live Wikipedia summaries.
See [setup](#setup) for more information on how to get started. Once you set this
up, you will be able to navigate around and read Wikipedia page summaries.

First, we fetch today's featured (English) Wikipedia page and set that as the 
root directory. If you navigate inside that directory, you will find a `00_content`
text file and more directories. Notice each directory is named according to its
title.

The `00_content` file stores a summary of the Wikipedia page. The rest of the directories
are other Wikipedia pages the current one links to. If you click into another directory,
you will see the same pattern of text file + more directories.

Note, some Wikipedia pages link to non-existant (deprecated, deleted) Wikipedia pages.
In such a case, if you navigate to a directory based on a non-existant Wikipedia page,
you will find it empty. This is expected behavior.

See [example](#example) for a more detailed walkthrough.

## Setup

1. Run `make setup`
   - this creates the `./mnt` directory, and attempts to setup a python virtual
   environment and download necessary dependencies.

2. Run `make wiki-browser`
   - this compiles necessary files for the Wikipedia Explorer Filesystem

3. Run `make mount` to start up the filesystem
   - this runs `./wiki-browser` with FUSE on `./mnt`, so that directory is the mountpoint.

4. Run `make clean` when done using the filesystem
   - this unmounts `./mnt` (`fusermount -u`).

## Example

1. Complete up until step 3 in [setup](#setup). Now, either open a file explorer or a new terminal and navigate to `./mnt` (the mounted directory).

2. **Visual:** In the mounted directory, you will see a directory titled today's featured Wikipedia article.  
   **Terminal:** Run `ls` to list the directory content (today's featured article).

3. **Visual:** Click into the directory.  
   **Terminal:** Run `cd <directory-name>` to navigate into the directory. Run `ls` to list the directory contents.  
   You will see `00_content` and additional directories, all with different names.

4. **Visual:** Open `00_content` (using Vim).  
   **Terminal:** Run `cat 00_content` to read the file in terminal.  
   This will display the summary of the Wikipedia page the directory represents.

5. **Visual:** Open another directory. Or click back up to the parent directory.  
   **Terminal:** Run `cd <directory-name>` to navigate into the directory. Run `ls` to list the directory contents. You can navigate back up to the parent folder by running `cd ..`.
   You will see `00_content` and additional directories, all with different names. Continue this process to navigate to other Wikipedia pages.

6. When finished, in the project directory run `make clean` so `./mnt` is unmounted.

## Notes

- Wikipedia data is fetched and downloaded utilizing the `wikipediaapi` Python package. It is all live, and the root directory matches the daily featured article (so changes regularly).
- The featured article title comes from the Wikipedia REST API; `requests` is used for that call.
- We preprocess titles before displaying them in the filesystem to only include English (ascii) titles.
- We chose to "option B" for the project; this means we support reading content from the filesystem in a hierarchical manner (Wikipedia page -> linked pages), but do not support writing to the filesystem.