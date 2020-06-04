# Contributing

Thanks for taking interest in contributing!

If you think that the extension is missing some useful features which would
benefit a lot of developers, you can open an issue describing your use case or
discuss the ways of implementing such features if you want to do it yourself.

First of all, it would be good for you to checkout the Godot Engine's
[CONTRIBUTING](https://github.com/godotengine/godot/blob/master/CONTRIBUTING.md)
document as the project shares similar ideas. Everything below outlines the
differences.

## To contribute, or not to contribute, that is the question

In order to properly answer this question, lets talk about statistics and take
silly estimates for comparison sake. :)

For instance, if you're developing a new feature directly in Godot Engine, the
proposed feature must be useful for at least 70% of the users (either 2D or 3D).

Now, if you're developing a new feature in Goost, the feature must be useful for
at least **30%** of the users, directly or indirectly.

A lot of the existing Goost functionality is a result of not approved and
not-yet-approved proposals by Godot Engine's core developers but which received
a general positive feedback from the community, or features which were removed
in previous versions of Godot. Such a feature can be considered as the best
candidate for being included as part of the Goost extension.

Another good reason to contribute is **performance**. If you think that GDScript
is very slow for your use cases, you may consider re-implementing a feature in
C++ if you think that it will benefit other developers.

If you need to use a feature for which there's an existing C/C++ library which
can be bundled as part of Goost, that's also a justifiable reason to contribute.

If none of this applies to you, chances are that you don't need to contribute to
Goost directly as a feature can (and likely should) be implemented via GDScript,
especially if the feature can be implemented in various ways which would only
benefit a handful of users. Instead, consider contributing to a project such as
[`Godot Node Extensions`](https://github.com/godot-extended-libraries/godot-next)
which aims to fulfill this need.

If you really want a feature to be included in Goost despite all of the above,
it might make sense to implement it as a module under the `modules/` directory.
But bear in mind that users will need to enable extra modules support manually,
and the complexity of the added features is always taken into account. Likewise,
if a new feature is specific to a single genre of games (or domains), it
**must** be implemented as a module.

If you're an independent Godot Engine C++ module developer who wants
to link an existing module to be included as part of Goost, feel free to open a
pull request linking your module as a `git submodule`. This might be the best
option if you'd like to have more freedom over your module's development.

### Third-party

As mentioned above, binding to third-party libraries is acceptable and they must
be compatible with the MIT license.
