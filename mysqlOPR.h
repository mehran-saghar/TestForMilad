
#ifndef __MYSQLOPR_H__
#define __MYSQLOPR_H__

#include "generic.h"
#include "mysql.h"
//#include "DICOMhd.h"

// macros defined for null checking and working with pointers
#define IS_NULL(x) (x == 0)
#define EMP2NULL(x) x.empty() ? 0 : x.c_str()
#define RELEASE(x) if(x) {free(x);x=0;}
#define RELEASE2(x) if(x) {delete x;x=0;}
#define STR_ASSIGN(x,y) if(y){x=(char*)malloc(strlen(y)+1);strcpy(x,y);} else x=0;
#define SET_MAXLEN_STRING(x, y, l) if (l > strlen(y)) strcpy(x, y); \
								   else { strncpy(x, y, l - 1); x[l - 1] = '\0';}

// macros defined for working with types
#define TYPE_IS_VALID(x) (0 <= x && x <= 23)
#define TYPE_IS_VARIANT(x) (x == 23)
#define TYPE_IS_STRING(x) (14 <= x && x <= 21)
#define TYPE_IS_DATE(x)	(10 <= x && x <= 12)
#define TYPE_IS_INTEGER(x) ((0 <= x && x <= 4) || x == 7)
#define TYPE_IS_NUMERAL(x) (0 <= x && x <= 7)

#define QUERY_BUFF_SIZE 64000

// This enum will be used to specify the different field types in tables 
enum DBTYPE
{
	dbtTinyInt = 0,			    //char            sql/TinyInt	      -128 < mysql/TINYINT < 127
	dbtSmallInt = 1,			//short			  sql/SmallInt      -32,768 < mysql/SMALLINT < 32,767
	dbtInt = 2,				    //long			  sql/Int		        -2,147,483,648 < mysql/INTEGER,MEDIUMINT,INT < 2,147,483,648
	dbtDecimal = 3,			    //double		  sql/Decimal       mysql/DECIMAL,DEC,NUMERIC.FIXED
	dbtBigInt = 4,				//long long	      sql/BigInt	      -9,223,372,036,854,775,808 < mysql/BIGINT < 9,223,372,036,854,775,807

	dbtFloat = 5,				//float	    	  sql/Float         mysql/FLOAT
	dbtReal = 6,				//double	      sql/Real	        mysql/DOUBLE,DOUBLE PRECISION,REAL

	dbtBit = 7,				    //bool		      sql/Bit		    mysql/BIT,BOOL,BOOLEAN

	dbtMoney = 8,				//???	          sql/Money	        ------------------------
	dbtSmallMoney = 9,			//???	    	  sql/SmallMoney    ------------------------

	dbtDateTime = 10,			//Array of bytes  sql/DateTime      mysql/DATETIME
	dbtSmallDateTime = 11,	    //Array of bytes  sql/SmallDateTime	mysql/DATE
	dbtTimeStamp = 12,			//Array of bytes  sql/TimeStamp	    mysql/TIMESTAMP

	dbtImage = 13,				//???             sql/Image		    ------------------------

	dbtNChar = 14,				//string		  sql/NChar	        ------------------------
	dbtChar = 15,				//char* 		  sql/Char	        mysql/CHAR < 255 length
	dbtNText = 16,				//string		  sql/NText	        ------------------------
	dbtText = 17,				//char* 		  sql/Text	        mysql/VARCHAR < 255 length
	dbtNVarChar = 18,			//string		  sql/NVarChar	    ------------------------
	dbtVarChar = 19,			//char* 		  sql/VarChar	    mysql/VARCHAR < 255 length

	dbtUniqueIdentifier = 20,	//char*			  sql/UniqueIdetifier	----------------------

	dbtVarBinary = 21,			//Array of bytes  sql/VarBinary	    mysql/BLOB,TINYBLOB,MEDIUMBLOB,LONGBLOB
                                //                                  mysql/TEXT,TINYTEXT,MEDIUMTEXT,LONGTEXT
	dbtBinary = 22,				//Array of bytes  sql/Binary	    mysql/BLOB,TINYBLOB,MEDIUMBLOB,LONGBLOB
                                //                                  mysql/TEXT,TINYTEXT,MEDIUMTEXT,LONGTEXT
	dbtVariant = 23				// void*		  any type
};

class CConnection;

class CFieldAttrs
{
	friend class CConnection;
	bool					m_bPK;
	bool					m_bInsertable;
	ANY						m_pDefVal;
	char*					m_strName;
	DBTYPE					m_type;
	unsigned long			m_ulMaxLen;
	bool					m_bNullable;
	bool					m_bUnsigned;
	char*					m_strDescription;

public:
  // the actual value of the field
	ANY						m_pVal;
  // constructor
	CFieldAttrs(const char* strName, DBTYPE type,
							bool bPK = false, bool bInsertable = true,
							bool bNullable = true, unsigned long ulMaxLen = 0,
							bool bUnsigned = false, ANY defaultValue = ANY(),
							const char* strDescription = 0/*, DcmTagKey* tagkey = 0*/);
  // destructor
	virtual ~CFieldAttrs();

  // get the valus of field's parameter
	bool					GetPK()						{return m_bPK;}
	bool					GetInsertable()		{return m_bInsertable;}

	ANY						GetDefaultValue()	{return m_pDefVal;}

	const char*				GetName()					{return m_strName;}
	DBTYPE					GetType()					{return m_type;}
	unsigned long			GetMaxLength()		{return m_ulMaxLen;}
	bool					GetNullable()			{return m_bNullable;}
	bool					GetUnsigned()			{return m_bUnsigned;}
	const char*				GetDescription()	{return m_strDescription;}
};

class CFieldAttrsCollection
{
	friend class CBaseType;
	CFieldAttrsCollection() {}
	~CFieldAttrsCollection() {m_col.Clear(true);}
	
	_baseCollection<CFieldAttrs> m_col;

public:
	void Add(CFieldAttrs* pfa) {m_col.Add(pfa);}
	CFieldAttrs* Remove(unsigned index) {return m_col.Remove(index);}
	CFieldAttrs* GetItem(unsigned index) {return m_col.GetItem(index);}
	CFieldAttrs* GetItem(const char* index) {
		for (unsigned i = 0; i < m_col.GetCount(); i++)
		{
			CFieldAttrs* pfa = m_col.GetItem(i);
			if (strcmp(pfa->GetName(), index) == 0)
				return pfa;
		}
		return 0;
	}
	long GetSize() {return m_col.GetCount();}

	CFieldAttrs* operator[](unsigned index) {return GetItem(index);}
	CFieldAttrs* operator[](const char* index) {return GetItem(index);}
};

class CBaseType 
{
	friend class CConnection;
	bool m_bHaveTable;		// determine whether this is an instance of a table or not
	CConnection* m_pcnn;	// the CConnection object without comment

protected:
	CFieldAttrsCollection* m_faFields;
	char* m_strTableName;	// the table name for derived table classes
	CBaseType(CConnection* pcnn);

public:
	CConnection* GetConnection() {return m_pcnn;}
	CBaseType(CConnection* pcnn, long count);
	virtual ~CBaseType() {
//    int iItems = m_faFields->GetSize();
//    for (int i = 0; i < iItems; i++)
//      delete m_faFields->GetItem(i);
		RELEASE(m_strTableName);
    RELEASE2(m_faFields);
	}
	ANY GetValue(int index) {return m_faFields->GetItem(index)->m_pVal;}
	ANY GetValue(const char* index) {return m_faFields->GetItem(index)->m_pVal;}
	void SetValue(int index, ANY val) {m_faFields->GetItem(index)->m_pVal = val;}
	void SetValue(char* index, ANY val) {m_faFields->GetItem(index)->m_pVal = val;}
	ANY operator[](int index) {return GetValue(index);}

	ANY operator[](char* index) {return GetValue(index);}
	const char* GetTableName() {return m_strTableName;}
	long GetFieldsCount() {return m_faFields->GetSize();}
	bool GetFromPKs(ANY* pks);
	bool GetFromPKs(ANY pk);

	virtual bool Insert();
	virtual bool Delete();
	virtual bool Delete(char* where);
	virtual bool Update();
	virtual bool Update(char* where);

protected:
	virtual void InitFields() {}

private:
	void InitFields(long count) 


	{
		while (count--) 
			m_faFields->Add(new CFieldAttrs(0, dbtVariant));
	}
};

/* the collection of CBaseTypes */
class CBaseTypeCollection
{
	_baseCollection<CBaseType> m_col;

public: 
	CBaseTypeCollection() {}
	~CBaseTypeCollection() {}

	void Add(CBaseType* pbt) {m_col.Add(pbt);}
	CBaseType* Remove(unsigned index) {return m_col.Remove(index);}
	CBaseType* GetItem(unsigned index) {return m_col.GetItem(index);}
	CBaseType* operator[](unsigned index) {return GetItem(index);}
	long GetCount() {return m_col.GetCount();}
	/* Clear the collection's list and delete all of its pointers */
	void Clear() {m_col.Clear(true);}
};

// the CConnection class for managing and stablishing the connection to mysql server
class CConnection
{
public:
	CConnection(const char* host = 0,
							const char* user = 0,
							const char* password = 0,
							const char* database = 0,
							const char* socketfile = 0,
							unsigned int port = 0);
	~CConnection();
	bool Init(const char* host,
						const char* user,
						const char* password,
						const char* database,
						const char* socketfile,
						unsigned int port = 0);

  // check whether the connection is stablished or not
	bool IsConnected();
  
  // get the error number and text from mysql
	const char* GetError(unsigned int* puiNumber);
  
  // these methods return the parameters of the connection
	const char* GetHost() {return m_pHost;}
	const char* GetUser() {return m_pUser;}
	const char* GetDatabase() {return m_pDatabase;}
	const char* GetSocketFile() {return m_pSocketFile;}
	unsigned int GetPort() {return m_uiPort;}

  // here are 4 query methods that do the desired operations:
  
  // 1- the base query method of the class that returns a collection of the desired type of table classes
  // the parameters are:
  // query: the actual query string;
  // table: if the query string is a table name or a standard sql text;
  // T: is the type of the CBaseType driven class that must return;
  // NOTE: the code for this method is included in header because of technical problems!!!
  template<class Tt> CBaseTypeCollection* ExecuteQuery(const char* query, bool table = false)
	{
		MYSQL_RES* res;
		MYSQL_ROW row;
		MYSQL_FIELD* fields;
		unsigned iFields;
		char newQuery[QUERY_BUFF_SIZE];

		if (table)
		{
			strcpy(newQuery, "SELECT * FROM ");
			strcat(newQuery, query);
		}
		else
		{
			strcpy(newQuery, query);
		}

		// Executes the query and check if it failed

		if (!Execute(newQuery))
			return 0;

		if (mysql_field_count(&m_mysql) == 0)
			return 0;

		if (IS_NULL((res = mysql_store_result(&m_mysql))))
			return 0;

		CBaseTypeCollection* btc = new CBaseTypeCollection();

		if ((iFields = mysql_num_fields(res)) == 0)
			return btc;

		fields = mysql_fetch_fields(res);
		while ((row = mysql_fetch_row(res)))
		{
			CBaseType* bt = new Tt(this);
			for (unsigned i = 0; i < iFields; i++)
			{
				CFieldAttrs* pfa = bt->m_faFields->GetItem(i);
				pfa->m_pVal = row[i];
			}
			btc->Add(bt);
		}
		mysql_free_result(res);
		return btc;
	}
  // 2- another type of query, but the return value is always of the type of CBaseType
  CBaseTypeCollection* ExecuteQuery(const char* query);
  // 3- the query that returns nothing
	bool Execute(const char* query);
  // 4- execute a query and returns a value
	ANY ExecuteScalar(const char* query);

  // a mappint routine to convert a mysql type to our defined type
	static DBTYPE CT(enum_field_types type);
  
private:
	MYSQL		m_mysql;
	char*		m_pHost;
	char*		m_pUser;
	char*		m_pDatabase;
	char*		m_pSocketFile;
	unsigned	m_uiPort;
	bool		m_bInitialized;

	MYSQL* get_sqlHandle() {return &m_mysql;}
};

class CSqlText
{
	char* pReturn;
	bool allocated;
public:
	CSqlText() : pReturn(0), allocated(false) {}
	~CSqlText() {if (pReturn && allocated) free(pReturn);}
	char* GetValueText(ANY& any, DBTYPE type)
	{
		if (pReturn && allocated)
			free(pReturn);

		pReturn = 0;
		allocated = false;
		if (any.empty())
		{
			pReturn = (char*)malloc(5);
			strcpy(pReturn, "NULL");
			allocated = true;
		}

		else if (TYPE_IS_STRING(type) || TYPE_IS_DATE(type))
		{
			pReturn = (char*)malloc(strlen((char*)any) + 3);
			strcpy(pReturn, "'");
			strcat(pReturn, (char*)any);
			strcat(pReturn, "'");
			allocated = true;
		}
		else
		{
			pReturn = (char*)any;
		}
		return pReturn;
	}
};

#endif
