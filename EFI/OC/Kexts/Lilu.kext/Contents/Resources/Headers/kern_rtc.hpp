a
    H=d/ �                   @  s6  U d Z ddlmZ ddlmZ ddlZddlmZ ddlZddl	Z	ddl
Z
ddlmZmZmZmZmZmZmZmZ ddlZddlmZ ddlZddlmZ dd	lmZ dd
lmZmZm Z m!Z!m"Z" ddl#m$Z$m%Z%m&Z&m'Z' ddl(m)Z)m*Z*m+Z+ ddl,m-Z- ddl.m/Z/m0Z0m1Z1m2Z2 ddl3m4Z4m5Z5m6Z6m7Z7m8Z8m9Z9m:Z: ddl;m<Z< ddl=m>Z> ddl?m@Z@ ddlAmBZB ddlCmDZD ddlEmFZF ddlGmHZH e�r�ddlmIZI dZJdZKdZLdZMdZNdZOdeK� deL� deM� deN� deFd  d! � deFd" � d#eO� d$�ZPd%eK� deL� d&�ZQd'eK� deL� deM� deFd  � deFd" � d(eO� d�ZRg d)�ZSe�d*d+d+�ZTd,eUd-< d.d/�d0d1�ZVd.d2d.d3�d4d5�ZWd6ZXd,eUd7< d8ZYd,eUd9< d:ZZd,eUd;< d<Z[d,eUd=< d>Z\d,eUd?< d@d@dA�dBdC�Z]G dDdE� dE�Z^G dFdG� dGe^�Z_G dHdI� dI�Z`G dJdK� dK�ZaG dLdM� dMeaejb�Zce)eP�e*dd!gdN�d�dRdSdSdTdSdSdUdSdVdSdWdXdYdZ�d[d\���Zdd2d2d]�d^d_�Zed`dad`db�dcdd�Zfd2dedf�dgdh�Zgdidjdidk�dldm�Zhded.dadn�dodp�Zid�d.dadSd2dr�dsdt�Zje+eFd" eFdu dv dw�G dxdy� dyea��Zkded.dSdadz�d{d|�Zld}dad~db�dd��ZmG d�d�� d��ZnG d�d�� d�ek�ZoG d�d�� d�eo�ZpdS )�a�  
Module contains tools for processing Stata files into DataFrames

The StataReader below was originally written by Joe Presbrey as part of PyDTA.
It has been extended and improved by Skipper Seabold from the Statsmodels
project who also developed the StataWriter and was finally added to pandas in
a once again improved version.

You can find more information on http://presbrey.mit.edu/PyDTA and
https://www.statsmodels.org/devel/
�    )�annotations)�abcN)�BytesIO)�IO�TYPE_CHECKING�Any�AnyStr�Final�Hashable�Sequence�cast)�relativedelta)�infer_dtype)�max_len_string_array)�CompressionOptions�FilePath�
ReadBuffer�StorageOptions�WriteBuffer)�CategoricalConversionWarning�InvalidColumnName�PossiblePrecisionLoss�ValueLabelTypeMismatch)�Appender�deprecate_nonkeyword_arguments�doc)�find_stack_level)�ensure_object�is_categorical_dtype�is_datetime64_dtype�is_numeric_dtype)�Categorical�DatetimeIndex�NaT�	Timestamp�isna�to_datetime�to_timedelta)�BooleanDtype)�IntegerDtype)�	DataFrame)�Index)�Series)�_shared_docs)�
get_handle)�Literalz�Version of given Stata file is {version}. pandas supports importing versions 105, 108, 111 (Stata 7SE), 113 (Stata 8/9), 114 (Stata 10/11), 115 (Stata 12), 117 (Stata 13), 118 (Stata 14/15/16),and 119 (Stata 15/16, over 32,767 variables).z�convert_dates : bool, default True
    Convert date variables to DataFrame time values.
convert_categoricals : bool, default True
    Read value labels and convert columns to Categorical/Factor variables.a  index_col : str, optional
    Column to set as index.
convert_missing : bool, default False
    Flag indicating whether to convert missing values to their Stata
    representations.  If False, missing values are replaced with nan.
    If True, columns containing missing values are returned with
    object data types and missing values are represented by
    StataMissingValue objects.
preserve_dtypes : bool, default True
    Preserve Stata datatypes. If False, numeric data are upcast to pandas
    default types for foreign data (float64 or int64).
columns : list or None
    Columns to retain.  Columns will be returned in the given order.  None
    returns all columns.
order_categoricals : bool, default True
    Flag indicating whether converted categorical data are ordered.zzchunksize : int, default None
    Return StataReader object for iterations, returns chunks with
    given number of lines.z=iterator : bool, default False
    Return StataReader object.z�Notes
-----
Categorical variables read through an iterator may not have the same
categories and dtype. This occurs when  a variable stored in a DTA
file is associated to an incomplete set of value labels that only
label a strict subset of the values.a>  
Read Stata file into DataFrame.

Parameters
----------
filepath_or_buffer : str, path object or file-like object
    Any valid string path is acceptable. The string could be a URL. Valid
    URL schemes include http, ftp, s3, and file. For file URLs, a host is
    expected. A local file could be: ``file://localhost/path/to/table.dta``.

    If you want to pass in a path object, pandas accepts any ``os.PathLike``.

    By file-like object, we refer to objects with a ``read()`` method,
    such as a file handle (e.g. via builtin ``open`` function)
    or ``StringIO``.
�
Zdecompression_options�filepath_or_buffer�storage_optionsz�

Returns
-------
DataFrame or StataReader

See Also
--------
io.stata.StataReader : Low-level reader for Stata data files.
DataFrame.to_stata: Export Stata data files.

a   

Examples
--------

Creating a dummy stata for this example
>>> df = pd.DataFrame({'animal': ['falcon', 'parrot', 'falcon',
...                              'parrot'],
...                   'speed': [350, 18, 361, 15]})  # doctest: +SKIP
>>> df.to_stata('animals.dta')  # doctest: +SKIP

Read a Stata dta file:

>>> df = pd.read_stata('animals.dta')  # doctest: +SKIP

Read a Stata dta file in 10,000 line chunks:
>>> values = np.random.randint(0, 10, size=(20_000, 1), dtype="uint8")  # doctest: +SKIP
>>> df = pd.DataFrame(values, columns=["i"])  # doctest: +SKIP
>>> df.to_stata('filename.dta')  # doctest: +SKIP

>>> itr = pd.read_stata('filename.dta', chunksize=10000)  # doctest: +SKIP
>>> for chunk in itr:
...    # Operate on a single chunk, e.g., chunk.mean()
...    pass  # doctest: +SKIP
z�Reads observations from Stata file, converting them into a dataframe

Parameters
----------
nrows : int
    Number of lines to read from data file, if None read whole file.
z

Returns
-------
DataFrame
z�Class for reading Stata dta files.

Parameters
----------
path_or_buf : path (string), buffer or path object
    string, path object (pathlib.Path or py._path.local                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   