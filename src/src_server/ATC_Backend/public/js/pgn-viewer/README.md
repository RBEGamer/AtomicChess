# In a Nutshell

You will find the detailed documentation about pgn-viewer (formerly named PgnViewerJS, which is a bad name for NPM) in the [top-level directory](https://github.com/mliebelt/PgnViewerJS/blob/master/readme.md).

## How to Install

`npm install` to get all dependencies resolved. It has as dependency the other module `pgn-reader` which is separately published on NPM.

## How to Build and Test

* `npm build`: Creates a new bundle `lib/pgnv.js` including the necessary `pgnv-assets` files copied to that directory.
* If you want to remove some locales, make a custom build by
  * first adjusting the list of locales in `.npmrc`
  * call locally first the script `gen_i18n`
  * then build the distribution

There are currently no UI tests available, I use the top-level directory `examples` to "test" changes in the application. You have to run a web server in the root directory of the module then.

## How to Use

You may include the viewer into an HTML page with roughly the following code.

    <html>
    <head>
      <script src="/lib/pgnv.js" type="text/javascript" ></script>
    </head>
    <body class="merida zeit">
      <div id="board"/>
      <script>
        PGNV.pgnView('board', {pgn: '1. e4 e5', locale: 'fr', width: '200px'});
      </script>
    </body>