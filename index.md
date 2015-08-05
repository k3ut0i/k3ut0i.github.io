---
layout: page
title : Welcome
posts : 0
---

About Me
========
I'm Keutoi. <img style="float: right" src="https://gravatar.com/avatar/1b0f4404b6fc8995cd6fea10b5c8b09c">
It's an alias but works online i suppose. I just completed my Bachelor's in
Electrical Engineering. I like learning new technologies reading fantasy and some parts of pure mathematics
and computer science. Most of my blog entries will be about these topics.

Disclaimer
==========
*   Language:   Though i've been studying in english as my medium of instruction for last 15 years,
    i haven't written much so my language maybe a bit crude with grammatical and spellingg errors.
    So any feedback is appreciated.
*   Content:    Some theoretical or technical content will be from some book, website or another
    blog. I will try to give credit where it is due. In case you don't feel the same about any post
    please let me know.

<div id="home">
    <h1>Blog Posts</h1>
    <ul class="posts">
    {% for post in site.posts %}
        <li><span>{{post.date | date_to_string }}</span> &raquo; <a href="{{site.baseurl }}{{ post.url }}">{{ post.title }}</a></li>
        {% endfor %}
    </ul>
</div>


[gravatar]: https://gravatar.com/avatar/1b0f4404b6fc8995cd6fea10b5c8b09c
