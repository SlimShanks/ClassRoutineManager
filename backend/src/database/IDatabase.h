#pragma once // prevention of the multiple inclusion oh the header file multiple

#include <QSqlDatabase> // database connection type
#include <QSqlQuery>    // handles the database queries
#include <QString>      // QT's string type
#include <QVariantList> // for handling different data types in database queries

namespace crm::database { // this namespace contains all the database related classes

/*   Interface :-> it list all the function every database driver must provide
 *  "virtual " -> ready to be override & " = 0 " both combine  then this became the pure virtual
 * funtion that means a subclass like SqliteDatabase  & MYSqlDatabase need to write its own version
 *  Its also the feature of polymerphism - also it helps in making the differnt implemetion instead
 * of the same base class function
 */

class Idatabase {
  public:
    virtual ~Idatabase() = default; // its the virtual destructor tha deletes the base pointer
                                    // safely without memeory leakage

    virtual bool open() = 0;         // opens the database connection
    virtual void close() = 0;        // close the connection
    virtual bool isOpen() const = 0; // checks if the database connection is open

    virtual bool migrate() = 0; // if the table doesnot exist then creates it

    virtual bool
    execute(const QString &sql,
            const QVariantList &params = {}) = 0; // its for the INSERT,UPDATE,DELETE where the
                                                  // "const QString& sql" -> sql command as text
    virtual QSqlQuery
    query(const QString &sql,
          const QVariantList &params = {}) = 0; // its for the operation like SELECT, return rows

    // returns the auto-increment id assigned by the last INSERT
    virtual int lastInsertId() const = 0;
};

} // namespace crm::database
