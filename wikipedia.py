import os
from datetime import UTC, datetime

import requests
from wikipediaapi import Wikipedia

# Wikipedia requires a User-Agent for requests
# https://foundation.wikimedia.org/wiki/Policy:Wikimedia_Foundation_User-Agent_Policy
USER_AGENT = (
    "Wikipedia Explorer Filesystem/1.0 (https://github.com/byebyelabs/wiki-filesystem)"
)


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


def save_wikipedia_page_links(page_title: str, output_dir: str):
    """Save list of page titles to a file."""

    # fetch list of page links
    links = _get_wikipedia_page_links(page_title)

    # clean up output file if it exists
    if os.path.exists(output_dir):
        os.remove(output_dir)

    # create output file and write list of page links
    with open(output_dir, "w") as f:
        f.write("\n".join(links))


def get_wikipedia_featured_title():
    """Get title of currently featured page on Wikipedia."""

    # format wikipedia API URL for today's featured article
    today = datetime.now(UTC).strftime("%Y/%m/%d")
    url = f"https://en.wikipedia.org/api/rest_v1/feed/featured/{today}"

    # fetch featured article data
    featured_data = requests.get(url, headers={"User-Agent": USER_AGENT})

    # extract featured article title from response
    featured_article_title = featured_data.json()["tfa"]["title"]
    return featured_article_title


if __name__ == "__main__":
    # get today's featured wikipedia page title
    print("> getting today's featured wikipedia page title")
    todays_featured_title = get_wikipedia_featured_title()
    print(f">>> {todays_featured_title}")

    # get page links on wikipedia page
    print(f"> getting page links on wikipedia page {todays_featured_title}")
    page_links = _get_wikipedia_page_links(todays_featured_title)
    print(f">>> found {len(page_links)} links")

    # save page links to file
    output_file = f".local.wiki.txt"
    print(f"> saving page links to file")
    save_wikipedia_page_links(todays_featured_title, output_file)
    print(f">>> saved to {output_file}")
