#include "AppShell.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QStackedWidget>
#include <QScrollArea>
#include <QButtonGroup>
#include <QStringList>

static QLabel *makeLabel(const QString &text, const char *objName) {
    auto *l = new QLabel(text);
    l->setObjectName(objName);
    return l;
}

// "Nguyen Minh Anh" -> "MA", "Le Hoang Phuc" -> "HP"
static QString initialsOf(const QString &name) {
    const QStringList parts = name.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 2) return name.left(2).toUpper();
    return (parts[parts.size() - 2].left(1) + parts.last().left(1)).toUpper();
}

AppShell::AppShell(Role role, const QString &userName, QWidget *parent)
    : QWidget(parent) {
    setObjectName("AppShell");
    setAttribute(Qt::WA_StyledBackground, true);
    setWindowTitle("UTH Student Attendance");
    setMinimumSize(1100, 700);

    m_stack = new QStackedWidget;
    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);

    auto *right = new QVBoxLayout;
    right->setSpacing(0);
    right->setContentsMargins(0, 0, 0, 0);
    right->addWidget(buildTopBar(role, userName));
    right->addWidget(m_stack, 1);
    right->addWidget(buildFooter());

    auto *root = new QHBoxLayout(this);
    root->setSpacing(0);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(buildSidebar(role));
    root->addLayout(right, 1);
}

QWidget *AppShell::buildSidebar(Role role) {
    auto *side = new QFrame;
    side->setObjectName("sidebar");
    side->setFixedWidth(240);
    auto *lay = new QVBoxLayout(side);
    lay->setContentsMargins(16, 24, 16, 20);
    lay->setSpacing(0);

    // Logo
    auto *logoRow = new QHBoxLayout;
    auto *mark = new QVBoxLayout;
    mark->setSpacing(4);
    mark->addWidget(makeLabel("UTH", "sbLogo"));
    auto *bar = new QFrame;
    bar->setObjectName("sbLogoBar");
    bar->setFixedSize(48, 3);
    mark->addWidget(bar);
    auto *txt = new QVBoxLayout;
    txt->setSpacing(0);
    txt->addWidget(makeLabel("Student", "sbAppName"));
    txt->addWidget(makeLabel("Attendance", "sbAppSub"));
    logoRow->addLayout(mark);
    logoRow->addLayout(txt);
    logoRow->addStretch();
    lay->addLayout(logoRow);

    // Nhan portal
    const QString portal = role == Role::Student ? "STUDENT PORTAL"
                         : role == Role::Lecturer ? "LECTURER PORTAL" : "ADMIN PORTAL";
    lay->addWidget(makeLabel(portal, "sbPortal"));

    // Vung nut menu (addPage se them vao day)
    m_navLayout = new QVBoxLayout;
    m_navLayout->setSpacing(6);
    lay->addLayout(m_navLayout);

    // Hop hoc ky
    auto *term = new QFrame;
    term->setObjectName("termBox");
    auto *tl = new QVBoxLayout(term);
    tl->setContentsMargins(14, 14, 14, 14);
    tl->setSpacing(4);
    tl->addWidget(makeLabel("CURRENT TERM", "termCaption"));
    tl->addWidget(makeLabel("Semester 1 · 2026–27", "termValue"));
    tl->addWidget(makeLabel("Attendance workspace", "termCaption"));
    lay->addSpacing(24);
    lay->addWidget(term);

    // Chan sidebar
    auto *uni = makeLabel("University of Transport Ho Chi Minh City", "sbFoot");
    uni->setWordWrap(true);
    auto *help = makeLabel("Need help? Contact academic affairs", "sbFoot");
    help->setWordWrap(true);
    lay->addSpacing(20);
    lay->addWidget(uni);
    lay->addSpacing(6);
    lay->addWidget(help);
    lay->addStretch();
    return side;
}

QWidget *AppShell::buildTopBar(Role role, const QString &userName) {
    auto *bar = new QFrame;
    bar->setObjectName("topBar");
    bar->setFixedHeight(68);
    auto *lay = new QHBoxLayout(bar);
    lay->setContentsMargins(24, 0, 24, 0);

    m_breadcrumb = makeLabel("", "breadcrumb");
    lay->addWidget(m_breadcrumb);
    lay->addStretch();

    auto *userBox = new QVBoxLayout;
    userBox->setSpacing(3);
    userBox->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *nameLabel = makeLabel(userName, "userName");
    nameLabel->setAlignment(Qt::AlignRight);
    userBox->addWidget(nameLabel);

    auto *links = new QHBoxLayout;
    links->setSpacing(8);
    links->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    const QString roleText = (role == Role::Admin) ? "ADMIN" : (role == Role::Lecturer ? "LECTURER" : "STUDENT");
    auto *roleBadge = makeLabel(roleText, "userRoleBadge");
    links->addWidget(roleBadge);

    auto *btnPw = new QPushButton("Change password");
    btnPw->setObjectName("linkPrimary");
    btnPw->setFlat(true);
    btnPw->setCursor(Qt::PointingHandCursor);
    links->addWidget(btnPw);

    auto *btnOut = new QPushButton("Sign out");
    btnOut->setObjectName("btnTopSignOut");
    btnOut->setCursor(Qt::PointingHandCursor);
    links->addWidget(btnOut);

    userBox->addLayout(links);
    lay->addLayout(userBox);

    auto *avatar = makeLabel(initialsOf(userName), "avatar");
    avatar->setFixedSize(38, 38);
    avatar->setAlignment(Qt::AlignCenter);
    lay->addSpacing(10);
    lay->addWidget(avatar);

    connect(btnPw, &QPushButton::clicked, this, &AppShell::changePasswordRequested);
    connect(btnOut, &QPushButton::clicked, this, &AppShell::signOutRequested);
    return bar;
}

QWidget *AppShell::buildFooter() {
    auto *f = new QFrame;
    f->setObjectName("shellFooter");
    auto *lay = new QHBoxLayout(f);
    lay->setContentsMargins(24, 10, 24, 10);
    lay->addWidget(makeLabel("UTH · Student Attendance System", "footText"));
    lay->addStretch();
    lay->addWidget(makeLabel("All times ICT (UTC+7)", "footText"));
    return f;
}

void AppShell::addPage(const QString &navText, QWidget *page) {
    // Boc trang trong QScrollArea de noi dung dai van cuon duoc
    auto *scroll = new QScrollArea;
    scroll->setObjectName("pageScroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(page);

    const int index = m_stack->addWidget(scroll);
    m_titles.append(navText);

    auto *btn = new QPushButton(navText);
    btn->setObjectName("navButton");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    m_navGroup->addButton(btn, index);
    m_navLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, [this, index] { showPage(index); });

    if (index == 0) {
        btn->setChecked(true);
        showPage(0);
    }
}

void AppShell::showPage(int index) {
    m_stack->setCurrentIndex(index);
    m_breadcrumb->setText("Academic operations / " + m_titles.value(index));
}

void AppShell::setCurrentPage(int index) {
    if (index >= 0 && index < m_titles.size()) {
        if (auto *btn = m_navGroup->button(index)) {
            btn->setChecked(true);
        }
        showPage(index);
    }
}

void AppShell::setCurrentPage(const QString &navText) {
    const int idx = m_titles.indexOf(navText);
    if (idx != -1) {
        setCurrentPage(idx);
    }
}
