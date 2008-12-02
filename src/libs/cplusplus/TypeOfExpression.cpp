/***************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
**
** Non-Open Source Usage
**
** Licensees may use this file in accordance with the Qt Beta Version
** License Agreement, Agreement version 2.2 provided with the Software or,
** alternatively, in accordance with the terms contained in a written
** agreement between you and Nokia.
**
** GNU General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the packaging
** of this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
**
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt GPL Exception
** version 1.2, included in the file GPL_EXCEPTION.txt in this package.
**
***************************************************************************/
***************************************************************************/

#include "TypeOfExpression.h"

#include <AST.h>
#include <TranslationUnit.h>
#include <cplusplus/LookupContext.h>
#include <cplusplus/ResolveExpression.h>

using namespace CPlusPlus;

TypeOfExpression::TypeOfExpression():
    m_ast(0)
{
}

void TypeOfExpression::setDocuments(const QMap<QString, Document::Ptr> &documents)
{
    m_documents = documents;
}

QList<TypeOfExpression::Result> TypeOfExpression::operator()(const QString &expression,
                                                             Document::Ptr document,
                                                             Symbol *lastVisibleSymbol)
{
    Document::Ptr expressionDoc = documentForExpression(expression);
    m_ast = extractExpressionAST(expressionDoc);

    m_lookupContext = LookupContext(lastVisibleSymbol, expressionDoc,
                                    document, m_documents);

    ResolveExpression resolveExpression(m_lookupContext);
    return resolveExpression(m_ast);
}

ExpressionAST *TypeOfExpression::ast() const
{
    return m_ast;
}

const LookupContext &TypeOfExpression::lookupContext() const
{
    return m_lookupContext;
}

ExpressionAST *TypeOfExpression::expressionAST() const
{
    return extractExpressionAST(m_lookupContext.expressionDocument());
}

ExpressionAST *TypeOfExpression::extractExpressionAST(Document::Ptr doc) const
{
    TranslationUnitAST *translationUnitAST = doc->translationUnit()->ast();

    // ### evaluate the expression
    ExpressionAST *expressionAST = 0;
    if (translationUnitAST) {
        DeclarationAST *declaration = translationUnitAST->declarations;
        SimpleDeclarationAST *simpleDecl = 0;
        if (declaration)
            simpleDecl = declaration->asSimpleDeclaration();
        if (simpleDecl && simpleDecl->decl_specifier_seq) {
            if (TypeofSpecifierAST *typeOfSpec = simpleDecl->decl_specifier_seq->asTypeofSpecifier())
                expressionAST = typeOfSpec->expression;
        }
    }
    return expressionAST;
}

Document::Ptr TypeOfExpression::documentForExpression(const QString &expression) const
{
    // create a __typeof__ specifier
    QByteArray declaration;
    declaration += "__typeof__ ";
    declaration += expression.toLatin1(); // C++ code needs to be in latin1
    declaration += ";";

    // create the expression's AST.
    Document::Ptr doc = Document::create(QLatin1String("<completion>"));
    doc->setSource(declaration);
    doc->parse();
    return doc;
}
