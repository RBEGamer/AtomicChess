#!/usr/bin/env runhaskell

{-
The MIT License (MIT)

Copyright (c) 2015 Wasif Hasan Baig <pr.wasif@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-}

{-|
  A Pandoc filter that replaces image or fenced code blocks and replaces them
  with rendered pandoc markdown tables.
  
  Image Links should have a "csv" extension.
  Include the csv file in markdown as
  
  > ![This is the table caption.](table.csv)
  
  Instead of image links, you can use fenced code blocks to reference an
  external CSV file using the "source" attribute.
  
  > ```{.table caption="This is the **caption**" source="table.csv"}
  > ```

  You can also omit the source attribute and include the contents of the CSV
  inside the code block directly.
  
  > ```{.table caption="This is the **caption**"}
  > Fruit, Quantity, Price
  > apples, 15, 3.24
  > oranges, 12, 2.22
  > ```
  
  You can include Pandoc Markdown in the CSV file. It will be parsed
  by the Pandoc Markdown Reader when the table is inserted in the document.
  
  For a detailed explanation of usage and options, see <https://github.com/baig/pandoc-csv2table-filter/blob/master/README.md README>
  at project's source directory.
-}

import Text.CSV         (CSV, parseCSV, parseCSVFromFile)
import Data.List
import Data.Text        (pack, unpack, justifyLeft, justifyRight, center)
import Text.Pandoc      (readMarkdown, def)
import qualified Text.Pandoc.JSON as J

-- Definitions ------------------------------------------------------------

-- Type synonyms
type Span      = Int
type Width     = Int
type Gutter    = Int
type Lines     = [String]
type Caption   = String
type AtrName   = String
type AtrValue  = String
type Atrs      = [(AtrName, AtrValue)]

-- | Type of the 'Table'.
data TableType = Simple    -- Simple Table
               | Multiline -- Multiline Table
               | Grid      -- Grid Table
               | Pipe      -- Pipe Table
               deriving (Eq, Show)

-- | Position of the caption.
data CaptionPos = BeforeTable -- Insert caption before table markdown.
                | AfterTable  -- Insert caption after table markdown.
                deriving (Show)

-- | Alignment of a Column in the Table.
--   Not all TableTypes support column alignments.
data Align = LeftAlign    -- Left Align
           | RightAlign   -- Right Align
           | CenterAlign  -- Center Align
           | DefaultAlign -- Default Align
           deriving (Show)

-- | A cell in a table has column span, cell width, cell alignment and the
--   number of lines.
--   
--     * __Span:__  Number of lines spanned by the cell.
--     * __Width:__ Width of the column this cell is contained inside
--     * __Align:__ Alignment of the content inside the cells
--     * __Lines:__ A list of strings where each string represents a line
data Cell = Cell Span Width Align Lines
            deriving (Show)
            
-- | A Row contains a list of Cells.
data Row = Row [Cell]
           deriving (Show)

-- | A Column contain information about its width and alignment.
--   
--     * __Width:__  Character length of the widest 'Cell' in a 'Column'.
--     * __Align:__  Alignment of the cells inside this column
data Column = Column Width Align
              deriving (Show)

-- | A Header contains a Row if present, otherwise NoHeader.
data Header = Header Row
            | NoHeader
            deriving (Show)

-- | A Table has a caption, information about each column's width and
--   alignment, either a header with a row or no header, and a series of rows.
data Table = Table Caption [Column] Header [Row]
             deriving (Show)

-- Builder Functions -----------------------------------------------------------

-- | Table Builder Functions
--   Helper functions to create a Table from the parsed CSV file (a list of
--   list of Strings).

-- | Converts Lines to Cell.
mkCell :: Align -> Lines -> [Cell]
mkCell a xs = map (Cell (span xs) 0 a) liness
            where
              span   = maximum . map (length . lines)
              liness = map lines xs

-- | Converts a list of Lines to a list of Cells.
mkCells :: [Align] -> [Lines] -> [[Cell]]
mkCells as xss = map (addCellAligns as .
                      addCellWidths columnWidths .
                      mkCell DefaultAlign) xss
               where
                 lines1       = map . map $ lines
                 calcWidths   = map . map . map $ (+2) . length
                 columnWidths = map maximum .
                                map concat  .
                                transpose   .
                                calcWidths  .
                                lines1      $
                                xss

addCellAligns :: [Align] -> [Cell] -> [Cell]
addCellAligns (a:as) (c:cs) = updateCellAlign a c  : addCellAligns as cs
addCellAligns []     (c:cs) = c : addCellAligns [] cs
addCellAligns (a:as) []     = []
addCellAligns []     []     = []

updateCellAlign :: Align -> Cell -> Cell
updateCellAlign a (Cell s w _ xs) = Cell s w a xs

addCellWidths :: [Width] -> [Cell] -> [Cell]
addCellWidths (w:ws) (c:cs) = updateCellWidth w c : addCellWidths ws cs
addCellWidths []     (c:cs) = c : addCellWidths [] cs
addCellWidths (w:ws) []     = []
addCellWidths []     []     = []

updateCellWidth :: Width -> Cell -> Cell
updateCellWidth w (Cell s _ a xs) = Cell s w a xs

mkRows :: [Align] -> [Lines] -> [Row]
mkRows as = map Row . mkCells as

mkColumns :: [Align] -> Row -> [Column]
mkColumns as (Row cs) = columnify as cs
                      where
                        columnify (a:as) ((Cell _ w _ _):cs) = Column w a  : columnify as cs
                        columnify []     ((Cell _ w _ _):cs) = Column w DefaultAlign : columnify [] cs
                        columnify (a:as) []                  = []
                        columnify []     []                  = []

mkTable :: Caption -> [Align] -> Bool -> [Lines] -> Table
mkTable c as h xss = case h of
                       True  -> Table c columns mkHeader $ tail $ mkRows as csv
                       False -> Table c columns NoHeader $ mkRows as csv
                   where
                     csv      = filter (/=[""]) xss
                     columns  = mkColumns as . head . mkRows as $ csv
                     mkHeader = Header $ head $ mkRows as csv

insertRowSeparator :: TableType -> [Column] -> Lines -> Lines
insertRowSeparator (Multiline) cs xs = intersperse "\n" xs
insertRowSeparator (Grid)      cs xs = intersperse (separator cs) xs
                                     where
                                       separator ((Column w _):cs) = "+" ++ replicate (w+2) '-' ++ separator cs
                                       separator []     = "+\n"
insertRowSeparator _           _  _  = []
                                                 
mkTableBorder :: TableType -> [Column] -> String
mkTableBorder (Multiline) ((Column w _):[]) = replicate w '-' ++ "\n"
mkTableBorder (Multiline) ((Column w _):cs) = replicate (w+1) '-' ++ mkTableBorder Multiline cs
mkTableBorder (Grid)      []                = "+\n"
mkTableBorder (Grid)      ((Column w _):cs) = "+" ++ replicate (w+2) '-' ++ mkTableBorder Grid cs
mkTableBorder _           _                 = ""

mkHeaderRowSeparator :: TableType -> [Column] -> String
mkHeaderRowSeparator (Grid) ((Column w _):cs) = "+" ++ replicate (w+2) '=' ++
                                                mkHeaderRowSeparator Grid cs
mkHeaderRowSeparator (Pipe) ((Column w a):cs) = (let sep = "|" ++ replicate (w+2) '-' 
                                                 in case a of
                                                     LeftAlign   -> "|:" ++ (drop 2 sep)
                                                     RightAlign  -> reverse $ ":" ++ (drop 1 . reverse $ sep)
                                                     CenterAlign -> ("|:" ++) $ drop 2 $ reverse $ ":" ++ (drop 1 . reverse $ sep)
                                                     _           -> reverse . drop 1. reverse $ sep ) ++
                                                mkHeaderRowSeparator Pipe cs
mkHeaderRowSeparator (Grid) []                = "+\n"
mkHeaderRowSeparator (Pipe) []                = "|\n"
mkHeaderRowSeparator t      ((Column w _):[]) = replicate w '-' ++ "\n"
mkHeaderRowSeparator t      ((Column w _):cs) = replicate w '-' ++ " " ++
                                                mkHeaderRowSeparator t cs

-- | Add members to pad cell content based on span
padCell :: Cell -> Cell
padCell (Cell s w a xs) = Cell s w a (xs ++ padList)
                        where
                          padList  = replicate padByNum (take w $ repeat ' ')
                          padByNum = s - length xs

-- | Expects padded cells
alignCellStrings :: TableType -> Cell -> Lines
alignCellStrings (Grid) (Cell _ w a cs) = map (alignText w LeftAlign) cs
alignCellStrings _      (Cell _ w a cs) = map (alignText w a) cs

cellToLines :: TableType -> Cell -> Lines
cellToLines t = alignCellStrings t . padCell

flatten :: [Lines] -> String
flatten = concatMap (++"\n") . map concat

addGutter :: Gutter -> Lines -> Lines
addGutter g (x:xs) = x : map ((replicate g ' ')++) xs

alignText :: Width -> Align -> String -> String
alignText w (LeftAlign)    = unpack . justifyLeft   w ' ' . pack
alignText w (RightAlign)   = unpack . justifyRight  w ' ' . pack
alignText w (CenterAlign)  = unpack . center        w ' ' . pack
alignText w (DefaultAlign) = unpack . justifyLeft   w ' ' . pack

row2Md :: TableType -> Row -> String
row2Md (Grid) (Row cs) = flatten $ transpose $ appendPipes $ map (cellToLines Grid) cs
row2Md (Pipe) (Row cs) = flatten $ transpose $ appendPipes $ map (cellToLines Pipe) cs
row2Md t      (Row cs) = flatten $ map (addGutter 1) $ transpose $ map (cellToLines t) cs

appendPipes :: [Lines] -> [Lines]
appendPipes (xs:[])  = [map (++" |") xs]
appendPipes (xs:xss) = map (("| "++) . (++" | ")) xs : (map (map (++" | ")) xss)

addCaption :: CaptionPos -> Caption -> String -> String
addCaption _             [] s = s
addCaption (BeforeTable) c  s = "Table: " ++ c ++ "\n\n" ++ s
addCaption (AfterTable)  c  s = s ++ "\nTable: " ++ c

toMarkdown :: TableType -> CaptionPos -> Table -> String
toMarkdown (Simple)    = toSimpleMd
toMarkdown (Multiline) = toMultilineMd
toMarkdown (Grid)      = toGridMd
toMarkdown (Pipe)      = toPipeMd

toMultilineMd :: CaptionPos -> Table -> String
toMultilineMd l (Table c cs (Header h) rs) = addCaption l c $
                                             mkTableBorder Multiline cs ++
                                             row2Md Multiline h ++
                                             mkHeaderRowSeparator Multiline cs ++
                                             (concatMap (++"") $
                                              insertRowSeparator Multiline cs $
                                              map (row2Md Multiline) rs) ++
                                             mkTableBorder Multiline cs
toMultilineMd l (Table c cs (NoHeader) rs) = addCaption l c $
                                             mkHeaderRowSeparator Multiline cs ++
                                             (concatMap (++"") $
                                              insertRowSeparator Multiline cs $
                                              map (row2Md Multiline) rs) ++
                                             mkHeaderRowSeparator Multiline cs

toGridMd :: CaptionPos -> Table -> String
toGridMd l (Table c cs (Header h) rs) = addCaption l c $
                                        mkTableBorder Grid cs ++
                                        row2Md Grid h ++
                                        mkHeaderRowSeparator Grid cs ++
                                        (concatMap (++"") $
                                         insertRowSeparator Grid cs $
                                         map (row2Md Grid) rs) ++
                                        mkTableBorder Grid cs
toGridMd l (Table c cs (NoHeader) rs) = addCaption l c $
                                        mkTableBorder Grid cs ++
                                        (concatMap (++"") $
                                         insertRowSeparator Grid cs $
                                         map (row2Md Grid) rs) ++
                                        mkTableBorder Grid cs

toPipeMd :: CaptionPos -> Table -> String
toPipeMd l (Table c cs (Header h) rs) = addCaption l c $
                                        row2Md Pipe h ++
                                        mkHeaderRowSeparator Pipe cs ++
                                        (concatMap (++"") $
                                         map (row2Md Pipe) rs)
toPipeMd l (Table c cs (NoHeader) rs) = addCaption l c $
                                        mkHeaderRowSeparator Pipe cs ++
                                        (concatMap (++"") $
                                         map (row2Md Pipe) rs)

toSimpleMd :: CaptionPos -> Table -> String
toSimpleMd l (Table c cs (Header h) rs) = addCaption l c $
                                          row2Md Simple h ++
                                          mkHeaderRowSeparator Simple cs ++
                                          (concatMap (++"") $
                                           map (row2Md Simple) rs) ++
                                          mkHeaderRowSeparator Simple cs
toSimpleMd l (Table c cs (NoHeader) rs) = addCaption l c $
                                          mkHeaderRowSeparator Simple cs ++
                                          (concatMap (++"") $
                                           map (row2Md Simple) rs) ++
                                          mkHeaderRowSeparator Simple cs


-- Helper Functions ------------------------------------------------------------

-- Helper functions to manipulate the Pandoc Document and parse the 
-- Configuration String.

-- | Add Inline from Image into Table as the caption
addInlineLabel :: [J.Inline] -> J.Pandoc -> J.Pandoc
addInlineLabel i (J.Pandoc m [(J.Table _ as ds ts tss)]) = J.Pandoc m [(J.Table i as ds ts tss)]
addInlineLabel _ x = x

-- | Extracts Blocks from Pandoc Document
toBlocks :: J.Pandoc -> [J.Block]
toBlocks (J.Pandoc _ bs) = bs

toTableType1 :: String -> TableType
toTableType1 (x:ys) = case x of
                       's' -> Simple
                       'm' -> Multiline
                       'p' -> Pipe
                       _   -> Grid
toTableType1 []     = Grid

toTableType :: String -> TableType
toTableType s = case s of
                  "simple"    -> Simple
                  "multiline" -> Multiline
                  "pipe"      -> Pipe
                  _           -> Grid

getTableType :: [J.Inline] -> TableType
getTableType ((J.Str s):[]) = toTableType1 s
getTableType (_:is)         = getTableType is
getTableType []             = Grid

-- | Whether to treat first line of CSV as a header or not.
isHeaderPresent :: [J.Inline] -> Bool
isHeaderPresent ((J.Str s):[]) = not $ "n" `isInfixOf` s
isHeaderPresent (_:is)         = isHeaderPresent is
isHeaderPresent []             = True

isHeaderPresent1 :: String -> Bool
isHeaderPresent1 ('n':'o':[]) = False
isHeaderPresent1 _            = True

toAlign :: String -> [Align]
toAlign (x:ys) = case x of
                   'l' -> LeftAlign    : toAlign ys
                   'L' -> LeftAlign    : toAlign ys
                   'r' -> RightAlign   : toAlign ys
                   'R' -> RightAlign   : toAlign ys
                   'c' -> CenterAlign  : toAlign ys
                   'C' -> CenterAlign  : toAlign ys
                   'd' -> DefaultAlign : toAlign ys
                   'D' -> DefaultAlign : toAlign ys
                   _   -> []          ++ toAlign ys
toAlign []     = []

-- | Parse Config String for alignment information
getAligns :: [J.Inline] -> [Align]
getAligns ((J.Str s):[]) = toAlign s
getAligns (_:is)         = getAligns is
getAligns []             = []

-- | Remove Str Inline from caption
removeConfigString :: [J.Inline] -> [J.Inline]
removeConfigString (_:[]) = []
removeConfigString (x:ys) = x : removeConfigString ys
removeConfigString []     = []

-- | Get value of attribute
getAtr :: AtrName -> Atrs -> AtrValue
getAtr a ((at,v):_) | a == at = v
getAtr a (_:xs)               = getAtr a xs
getAtr a []                   = ""

-- | Make Pandoc Table from Image Inline 
tableFromImageInline :: [J.Inline] -> CSV -> J.Pandoc
tableFromImageInline l = addInlineLabel (removeConfigString l) .
                         readMarkdown def .
                         toMarkdown (getTableType l) AfterTable .
                         mkTable "" (getAligns l) (isHeaderPresent l)

-- | Make Pandoc Table from Code Block 
tableFromCodeBlock :: Atrs -> CSV -> J.Pandoc
tableFromCodeBlock as = readMarkdown def .
                        toMarkdown (toTableType $ getAtr "type" as) AfterTable .
                        mkTable (getAtr "caption" as)
                                (toAlign $ getAtr "aligns" as)
                                (isHeaderPresent1 $ getAtr "header" as)

-- Main ------------------------------------------------------------------------

main :: IO ()
main = J.toJSONFilter tablifyCsvLinks

tablifyCsvLinks :: J.Block -> IO [J.Block]
tablifyCsvLinks (J.Para [(J.Image l (f, _))]) | "csv" `isSuffixOf` f = do
    csv <- parseCSVFromFile f
    case csv of
        (Left _)    -> return []
        (Right xss) -> return .
                       toBlocks .
                       tableFromImageInline l $
                       xss
tablifyCsvLinks b@(J.CodeBlock (_, cs, as) s) | "table" `elem` cs = do
    let file = getAtr "source" as
    case file of
      "" -> case s of
              "" -> return [b]
              _  -> case (parseCSV "" s) of
                      (Left _)    -> return []
                      (Right xss) -> return .
                                     toBlocks .
                                     tableFromCodeBlock as $
                                     xss
      _  -> do
              csv <- parseCSVFromFile file
              case csv of
                (Left _)    -> return []
                (Right xss) -> return .
                               toBlocks .
                               tableFromCodeBlock as $
                               xss
tablifyCsvLinks x = return [x]
