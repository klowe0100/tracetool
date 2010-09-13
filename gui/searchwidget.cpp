/**********************************************************************
** Copyright (C) 2010 froglogic GmbH.
** All rights reserved.
**********************************************************************/

#include "searchwidget.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>
#include <QStyle>
#include <QStyleOptionFrameV2>
#include <QVBoxLayout>

#include <assert.h>

UnlabelledLineEdit::UnlabelledLineEdit( QWidget *parent )
{
}

void UnlabelledLineEdit::setPlaceholderText( const QString &placeholderText )
{
    m_placeholderText = placeholderText;
    update();
}

void UnlabelledLineEdit::paintEvent( QPaintEvent *e )
{
    QLineEdit::paintEvent( e );
    if ( !hasFocus() && text().isEmpty() ) {
        QPainter p( this );
        QFont f = font();
        f.setStyle( QFont::StyleItalic );
        p.setFont( f );
        p.setPen( QPen( Qt::gray ) );

        QStyleOptionFrameV2 opt;
        initStyleOption( &opt );

        QRect r = style()->subElementRect( QStyle::SE_LineEditContents, &opt, this );

        p.drawText( r, Qt::AlignLeft | Qt::AlignVCenter, m_placeholderText );
    }
}

SearchWidget::SearchWidget( QWidget *parent )
    : QWidget( parent ),
    m_lineEdit( 0 ),
    m_buttonLayout( 0 )
{
    m_lineEdit = new UnlabelledLineEdit( this );
    connect( m_lineEdit, SIGNAL( textEdited( const QString & ) ),
             this, SLOT( termEdited( const QString & ) ) );
    m_lineEdit->setPlaceholderText( "Search trace data..." );

    m_strictMatch = new QRadioButton( tr( "Strict" ), this );
    m_strictMatch->setChecked( true );
    connect( m_strictMatch, SIGNAL( clicked() ),
             this, SLOT( emitSearchCriteria() ) );
    m_strictMatch->hide();
    m_wildcardMatch = new QRadioButton( tr( "Wildcard" ), this );
    connect( m_wildcardMatch, SIGNAL( clicked() ),
             this, SLOT( emitSearchCriteria() ) );
    m_wildcardMatch->hide();
    m_regexpMatch = new QRadioButton( tr( "RegExp" ), this );
    connect( m_regexpMatch, SIGNAL( clicked() ),
             this, SLOT( emitSearchCriteria() ) );
    m_regexpMatch->hide();

    // XXX Enable the radio button once regexp matching is
    // implemented in EntryItemModel
    m_regexpMatch->setEnabled( false );

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->setMargin( 0 );
    m_buttonLayout->setSpacing( 0 );

    m_modifierLayout = new QVBoxLayout;
    m_modifierLayout->setMargin( 0 );
    m_modifierLayout->setSpacing( 2 );
    m_modifierLayout->addWidget( m_strictMatch );
    m_modifierLayout->addWidget( m_wildcardMatch );
    m_modifierLayout->addWidget( m_regexpMatch );

    QGridLayout *layout = new QGridLayout( this );
    layout->setMargin( 0 );
    layout->addWidget( m_lineEdit, 0, 0 );
    layout->addLayout( m_buttonLayout, 1, 0 );
    layout->addLayout( m_modifierLayout, 0, 1, 2, 1 );
}

void SearchWidget::emitSearchCriteria()
{
    QStringList selectedFields;
    QList<QPushButton *>::ConstIterator it, end = m_fieldButtons.end();
    for ( it = m_fieldButtons.begin(); it != end; ++it ) {
        if ( ( *it )->isChecked() ) {
            selectedFields.append( ( *it )->text() );
        }
    }

    MatchType matchType;
    if ( m_strictMatch->isChecked() ) {
        matchType = StrictMatch;
    } else if ( m_wildcardMatch->isChecked() ) {
        matchType = WildcardMatch;
    } else if ( m_regexpMatch->isChecked() ) {
        matchType = RegExpMatch;
    } else {
        assert( !"Some match type radio button must be checked" );
    }

    emit searchCriteriaChanged( m_lineEdit->text(), selectedFields, matchType );
}

void SearchWidget::termEdited( const QString &newTerm )
{
    QList<QPushButton *>::ConstIterator it, end = m_fieldButtons.end();
    for ( it = m_fieldButtons.begin(); it != end; ++it ) {
        ( *it )->setVisible( !newTerm.isEmpty() );
    }
    m_strictMatch->setVisible( !newTerm.isEmpty() );
    m_wildcardMatch->setVisible( !newTerm.isEmpty() );
    m_regexpMatch->setVisible( !newTerm.isEmpty() );
    emitSearchCriteria();
}

void SearchWidget::setFields( const QStringList &fields )
{
    qDeleteAll( m_fieldButtons );
    m_fieldButtons.clear();

    int width = 0;

    QStringList::ConstIterator it, end = fields.end();
    for ( it = fields.begin(); it != end; ++it ) {
        QPushButton *fieldButton = new QPushButton( *it );
        connect( fieldButton, SIGNAL( clicked() ),
                 this, SLOT( emitSearchCriteria() ) );

        fieldButton->setCheckable( true );
        fieldButton->setChecked( true );

        QFont f = fieldButton->font();
        f.setPointSize( ( f.pointSize() * 90 ) / 100 );
        fieldButton->setFont( f );

        m_buttonLayout->insertWidget( 0, fieldButton );

        m_fieldButtons.append( fieldButton );

        width += fieldButton->sizeHint().width();

        fieldButton->hide();
    }

    setMinimumWidth( qMax( width, m_lineEdit->minimumWidth() ) );
}

