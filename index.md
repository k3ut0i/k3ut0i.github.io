---
layout: page
title : Welcome
posts : 0
---

Posts
========
<div id="home">
    <h1>Blog Posts</h1>
    <ul class="posts">
    {% for post in site.posts %}
        <li><span>{{post.date | date_to_string }}</span> &raquo; <a href="{{site.baseurl }}{{ post.url }}">{{ post.title }}</a></li>
        {% endfor %}
    </ul>
</div>


[gravatar]: https://gravatar.com/avatar/1b0f4404b6fc8995cd6fea10b5c8b09c
