from pprint import pprint

from wikipediaapi import Wikipedia


class WikipediaPageNotFoundError(Exception):
    """Raised when a Wikipedia page is not found."""

    def __init__(self, page_title: str):
        self.page_title = page_title
        super().__init__(f"Wikipedia page {page_title} not found!")


def get_wikipedia_page_links(source_page_title: str) -> list[str]:
    """Return unique links found on a Wikipedia page by title as a list of page titles."""
    # Wikipedia requires a User-Agent for requests
    # https://foundation.wikimedia.org/wiki/Policy:Wikimedia_Foundation_User-Agent_Policy
    user_agent = "Wikipedia Explorer Filesystem/1.0 (https://github.com/byebyelabs/wiki-filesystem)"

    # initialize the Wikipedia API client and read source page
    wiki = Wikipedia(user_agent=user_agent, language="en")
    page = wiki.page(source_page_title)

    # raise custom error if the wikipedia page does not exist
    if not page.exists():
        raise WikipediaPageNotFoundError(source_page_title)

    # return the list of page titles
    return [link.title for link in page.links.values()]


if __name__ == "__main__":
    # sample wikipedia page title test
    sample_page_title = "Despre_tine"
    page_links = get_wikipedia_page_links(sample_page_title)
    pprint(page_links)
