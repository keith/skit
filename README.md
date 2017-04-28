# skit

`skit` is a dead simple CLI for interacting with
[`sourcekit`][sourcekit].

## Usage

Here's an example of a request that returns the structure of the source:

Create a `request.yaml`:

```yaml
key.request: source.request.editor.open
key.name: "example.swift"
key.sourcetext: "struct Foo {}"
```

Run `skit`:

```sh
$ skit request.yaml
```

Get the response from `sourcekitd`:

```json
{
  "key.offset": 0,
  "key.length": 14,
  "key.diagnostic_stage": "source.diagnostic.stage.swift.parse",
  "key.syntaxmap": [
    {
      "key.kind": "source.lang.swift.syntaxtype.keyword",
      "key.offset": 0,
      "key.length": 6
    },
    {
      "key.kind": "source.lang.swift.syntaxtype.identifier",
      "key.offset": 7,
      "key.length": 3
    }
  ],
  "key.substructure": [
    {
      "key.kind": "source.lang.swift.decl.struct",
      "key.accessibility": "source.lang.swift.accessibility.internal",
      "key.name": "Foo",
      "key.offset": 0,
      "key.length": 13,
      "key.nameoffset": 7,
      "key.namelength": 3,
      "key.bodyoffset": 12,
      "key.bodylength": 0
    }
  ]
}
```

`sourcekit` supports many request types, for details on what you can do
see the [protocol documentation][protocol].

## Installation

### [Homebrew](https://brew.sh/)

```sh
$ brew install keith/formulae/skit --HEAD
```

NOTE: On macOS the installation is currently tied to your Xcode version,
so if you move or update Xcode you'll have to reinstall `skit`.

### [Docker](https://www.docker.com/) ([hub page][hub])

```sh
$ docker pull smileykeith/skit:TAG
```

NOTE: The tags for docker line up with the git tags on this repo.

### Manually

```sh
$ make install
```

If you'd like to link against a version of `sourcekit` at a different
location from the default (based on the default Xcode install on macOS,
and `/usr/lib` on Linux), you can build manually and provide some
configuration variables:

```sh
$ SOURCEKIT_DIRECTORY=/path/to/lib/including/sourcekit make install
```

Or to just use a different Xcode version (one that is not your default
set with `xcode-select`):

```sh
$ XCODE_PATH=/path/to/Xcode.app/Contents/Developer make install
```

See the `Makefile` for more configuration options.

## Resources

- The [protocol documentation][protocol] defines most of the requests
  you can use with `sourcekit`

- [`SourceKitten`](https://github.com/jpsim/SourceKitten/) provides a
  much better interface for specific requests, meaning you don't need to
  deal with the implementation details of `sourcekit`

- `sourcekit` is [open source!][sourcekit] so you can read the source
  for more details on how the pieces fit together

- JP Simard (the author of `SourceKitten`) has written some [blog
  posts](http://www.jpsim.com/uncovering-sourcekit/) and done [a
  talk](https://news.realm.io/news/appbuilders-jp-simard-sourcekit/)
  about how `sourcekit` works

- If you're interested in reproducing something that Xcode does, you can
  see the communication between Xcode and `sourcekitd` by setting
  `SOURCEKIT_LOGGING=3` and launching Xcode

[hub]: https://hub.docker.com/r/smileykeith/skit/
[protocol]: https://github.com/apple/swift/blob/master/tools/SourceKit/docs/Protocol.md
[sourcekit]: https://github.com/apple/swift/tree/master/tools/SourceKit
