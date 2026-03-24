import os
import sys
from datetime import UTC, datetime

import requests
from wikipediaapi import Wikipedia

# Wikipedia requires a User-Agent for requests
# https://foundation.wikimedia.org/wiki/Policy:Wikimedia_Foundation_User-Agent_Policy
USER_AGENT = (
    "Wikipedia Explorer Filesystem/1.0 (https://github.com/byebyelabs/wiki-filesystem)"
)

OUTPUT = "/home/bhattara/csc313/wiki-filesystem/.local.output.txt"


class WikipediaPageNotFoundError(Exception):
    """Custom error raised when a Wikipedia page is not found."""

    def __init__(self, page_title: str):
        self.page_title = page_title
        super().__init__(f"Wikipedia page {page_title} not found!")


def _get_wikipedia_page_links(source_page_title: str) -> list[str]:
    """Return unique links found on a Wikipedia page by title as a list of page titles."""

    # initialize Wikipedia API client and read source page
    wiki = Wikipedia(user_agent=USER_AGENT, language="en")
    page = wiki.page(source_page_title)

    # raise custom error if wikipedia page does not exist
    if not page.exists():
        raise WikipediaPageNotFoundError(source_page_title)

    # return list of page titles
    return [link.title for link in page.links.values()]


def _filter_wikipedia_titles(titles: list[str]) -> list[str]:
    """Filter and sort list of strings (titles); filters out non-English (ascii) titles."""
    filtered_titles: set[str] = set()
    for title in titles:
        title = title.strip().replace(" ", "_")

        # skip empty title
        if not title:
            continue

        # skip accents and different languages
        if not title.isascii():
            continue

        filtered_titles.add(title)

    # sorts and converts to list of strings
    return sorted(list(filtered_titles))


def _get_wikipedia_featured_title():
    """Get title of currently featured page on Wikipedia."""

    # format wikipedia API URL for today's featured article
    today = datetime.now(UTC).strftime("%Y/%m/%d")
    url = f"https://en.wikipedia.org/api/rest_v1/feed/featured/{today}"

    # fetch featured article data
    featured_data = requests.get(url, headers={"User-Agent": USER_AGENT})

    # extract featured article title from response
    featured_article_title = featured_data.json()["tfa"]["title"]
    return featured_article_title


def _get_wikipedia_page_summary(source_page_title: str) -> str:
    """Get summary of Wikipedia page by title."""

    # initialize Wikipedia API client and read source page
    wiki = Wikipedia(user_agent=USER_AGENT, language="en")
    page = wiki.page(source_page_title)

    # raise custom error if wikipedia page does not exist
    if not page.exists():
        raise WikipediaPageNotFoundError(source_page_title)

    return page.summary


def delete_dir(output_dir: str):
    # clean up output file if it exists
    if os.path.exists(output_dir):
        os.remove(output_dir)


def save_content_to_file(content: str, output_dir: str):
    # clean up output file if it exists
    delete_dir(output_dir)

    # create output file and write content
    with open(output_dir, "w") as f:
        f.write(content)


if __name__ == "__main__":
    arguments = sys.argv

    if len(arguments) == 2 and arguments[1] in ("--get-featured-title", "-ft"):
        print("getting today's featured title")
        featured_title = _get_wikipedia_featured_title()
        save_content_to_file(featured_title, OUTPUT)
    elif len(arguments) == 3 and arguments[1] in ("--get-page-links", "-gl"):
        page_title = arguments[2].split("/")[-1]
        print(f"getting links on wiki page {page_title}")

        try:
            links = _get_wikipedia_page_links(page_title)
            links = _filter_wikipedia_titles(links)
            links = "\n".join(links)
            save_content_to_file(links, OUTPUT)
        except WikipediaPageNotFoundError as wpnfe:
            delete_dir(OUTPUT)
            print(wpnfe)
    elif len(arguments) == 3 and arguments[1] in ("--get-page-summary", "-gs"):
        page_title = arguments[2].split("/")[-1]
        print(f"getting summary on wiki page {page_title}")

        try:
            summary = _get_wikipedia_page_summary(page_title)
            save_content_to_file(summary, OUTPUT)
        except WikipediaPageNotFoundError as wpnfe:
            delete_dir(OUTPUT)
            print(wpnfe)
    else:
        # improper usage
        raise Exception("improper arguments passed")
